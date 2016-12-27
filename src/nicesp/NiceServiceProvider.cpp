#include "NiceServiceProvider.hpp"
#include "SignalingConnection.hpp"
#include "GameSession.hpp"
#include <dplib/DPLobby.hpp>
#include <dplib/DPServiceProvider.hpp>
#include <stack>

using dplib::DPLobby;
using dplib::DPServiceProvider;

namespace nicesp {

const gchar* DEFAULT_SIGNALING_HOST = "localhost";
const guint DEFAULT_SIGNALING_PORT = 7788;

static void* mainLoopThread (void* ctx) {
  auto loop = static_cast<GMainLoop*>(ctx);
  CoInitialize(NULL);

  g_debug("[GMainLoop::mainLoopThread] loop start");
  g_main_loop_run(loop);
  g_debug("[GMainLoop::mainLoopThread] loop complete");

  CoUninitialize();
  g_thread_exit(NULL);
  return nullptr;
}

NiceServiceProvider::NiceServiceProvider(SPINITDATA* data)
    :
    DPServiceProvider(data),
    mainLoop(nicesp::mainLoop),
    enumSessionsSC(nullptr),
    currentEnumSessionsMessageId(0) {
};

HRESULT NiceServiceProvider::EnumSessions (void* message, int size, bool returnStatus) {
  if (!DPLobby::get()->getConnectionSettings()) {
    return DP_OK;
  }

  auto guid = DPLobby::get()->getConnectionSettings()
    ->getSessionDesc()->getSessionGuid();
  g_debug("[EnumSessions] guidInstance = %s", to_string(guid).c_str());

  if (!this->enumSessionsSC) {
    this->enumSessionsSC = new SignalingConnection(
      DEFAULT_SIGNALING_HOST, DEFAULT_SIGNALING_PORT
    );
    this->enumSessionsSC->onEnumSessionsResponse = [this] (auto data, auto size) {
      g_debug("[EnumSessions] Discovered session %d", size);
      this->handleMessage(data, size);
    };
  }
  this->enumSessionsSC->relayEnumSessions(guid);

  return DP_OK;
}

HRESULT NiceServiceProvider::Open (DPSP_OPENDATA* data) {
  const auto isHost = !!data->bCreate;

  g_thread_new("main loop", &mainLoopThread, this->mainLoop);

  g_debug("[NiceServiceProvider::Open] before this");
  if (this->enumSessionsSC) {
    delete this->enumSessionsSC;
    this->enumSessionsSC = NULL;
  }
  g_debug("[NiceServiceProvider::Open] after this");

  g_debug("[NiceServiceProvider::Open] new session (provider = %p)", this->getInternalServiceProvider());
  const auto host = DEFAULT_SIGNALING_HOST;
  const auto port = DEFAULT_SIGNALING_PORT;
  auto connection = new SignalingConnection(host, port);
  auto session = new GameSession(connection, isHost);

  this->session = session;

  auto sessionId = DPLobby::get()->getConnectionSettings()
    ->getSessionDesc()->getSessionGuid();
  connection->connect(sessionId, isHost);

  session->onMessage = [this] (const auto id, auto data, const auto size) {
    g_debug("[NiceServiceProvider::Open/onMessage] Receiving message (%d bytes) from %ld", size, id);
    this->replyTargets.push(id);

    g_debug("[NiceServiceProvider::Open/onMessage] HandleMessage (cmd = %02x)", reinterpret_cast<short*>(data)[2]);
    auto hr = this->handleMessage(data, size);
    g_debug("[NiceServiceProvider::Open/onMessage] Handled message (hr = %08lx %s)",
      hr, dplib::getDPError(hr).c_str());
    this->replyTargets.pop();
  };

  if (isHost) {
    connection->onEnumSessions = [this] (const auto id) {
      g_debug("[NiceServiceProvider::Open] onEnumSessions %d", id);

      // The signaling server asked us to send info about local sessions, so
      // create a fake ENUMSESSIONS message for DirectPlay.
      auto applicationGuid = DPLobby::get()
        ->getConnectionSettings()
        ->getSessionDesc()
        ->getApplicationGuid();
      auto message = new DPSPEnumSessionsMessage(applicationGuid);

      this->currentEnumSessionsMessageId = id;
      this->handleMessage(message, sizeof(*message), NULL);
    };
  } else {
    session->waitUntilConnectedWithHost();
    g_debug("[NiceServiceProvider::Open] Connected w/Host (provider = %p)", this->getInternalServiceProvider());
  }

  return DP_OK;
}

HRESULT NiceServiceProvider::CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  const auto isLocal = (data->dwFlags & DPLAYI_PLAYER_PLAYERLOCAL) != 0;
  const auto isNameServer = (data->dwFlags & DPLAYI_PLAYER_NAMESRVR) != 0;
  const auto isSystemPlayer = (data->dwFlags & DPLAYI_PLAYER_SYSPLAYER) != 0;

  if (isLocal && !isSystemPlayer) {
    // We are player $data->idPlayer
    this->session->setLocalPlayer(data->idPlayer);
  }
  if (isNameServer) {
    this->session->setNameServer(data->idPlayer);
    if (isLocal) {
      // We are the name server
      // TODO Do something here?
      // session->getSignalingConnection()
      //   ->authenticate(data->idPlayer);
    }
  }

  g_debug("[DPNice_CreatePlayer] maybe SetSPPlayerData");

  const auto player = this->session->getPlayerById(data->idPlayer);
  if (player) {
    g_debug("[DPNice_CreatePlayer] SetSPPlayerData (%p)", player);
    this->getInternalServiceProvider()
      ->SetSPPlayerData(data->idPlayer, player, sizeof(*player), DPSET_LOCAL);
  }

  return DP_OK;
}

HRESULT NiceServiceProvider::Send (DPSP_SENDDATA* data) {
  auto target = data->idPlayerTo;
  Player* player = nullptr;
  if (target == 0) {
    g_debug("[NiceServiceProvider::Send] Send to name server");
    // sizeof (DPMSG_CREATEPLAYERORGROUP) = 48
    player = this->session->getNameServerPlayer();
  } else {
    g_debug("[NiceServiceProvider::Send] Send to player %ld", target);
    player = this->session->getPlayerById(target);
  }

  if (!player) {
    g_warning("[NiceServiceProvider::Send] Player not found");
    return DPERR_UNAVAILABLE;
  }

  g_debug("[NiceServiceProvider::Send] player = %ld", player->id);

  player->send(data->dwMessageSize, data->lpMessage);

  return DP_OK;
}

HRESULT NiceServiceProvider::Reply (void* spHeader, void* message, int size, DPID nameServer) {
  g_debug("[NiceServiceProvider::Reply] currentEnumSessionsMessageId = %d", this->currentEnumSessionsMessageId);
  // LOL. Careful! Assuming here that this is indeed an enumsessions reply message.
  // FIXME Really actually only reply to enumsessions messages here. Perhaps by
  // parsing the message envelope to check.
  if (this->currentEnumSessionsMessageId != 0) {
    g_debug("[NiceServiceProvider::Reply] SignalingConnection (%p)", session->getSignalingConnection());
    this->session->getSignalingConnection()->relayEnumSessionsResponse(
      this->currentEnumSessionsMessageId,
      message,
      size
    );
    this->currentEnumSessionsMessageId = 0;
    return DP_OK;
  }

  if (this->replyTargets.empty()) {
    g_warning("[NiceServiceProvider::Reply] Unsure to whom to reply");
    return DPERR_UNAVAILABLE;
  }

  DPID target = this->replyTargets.top();

  g_debug("[NiceServiceProvider::Reply] Replying to %ld", target);
  auto player = this->session->getPlayerById(target);
  if (!player) {
    g_warning("[NiceServiceProvider::Reply] Could not find player");
    return DPERR_UNAVAILABLE;
  }

  player->send(size, message);
  return DP_OK;
}

HRESULT NiceServiceProvider::Close () {
  g_main_loop_quit(this->mainLoop);
  return DP_OK;
}

HRESULT NiceServiceProvider::Shutdown () {
  g_main_loop_unref(this->mainLoop);
  return DP_OK;
}

}
