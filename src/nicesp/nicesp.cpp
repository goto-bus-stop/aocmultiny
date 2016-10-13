#include "GameSession.hpp"
#include "nicesp.hpp"
#include "../util.hpp"
#include <dplib/DPLobby.hpp>
#include <gio/gnetworking.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "debug.hpp"

using namespace std;
using dplib::DPLobby;

namespace nicesp {

const gchar* DEFAULT_SIGNALING_HOST = "localhost";
const guint DEFAULT_SIGNALING_PORT = 7788;

GMainLoop* gloop;
map<void*, DPID> _replyTargetsHACK;
GameSession* _gameSession;
IDirectPlaySP* _providerHACK;

static GameSession* getGameSession (IDirectPlaySP* provider) {
  g_debug("[getGameSession] stub");
  return _gameSession;
}

/**
 * (Are Service Providers suppposed to access this? Well, it works, so…)
 */
GUID getSessionGuid () {
  g_debug("[getSessionGuid]");
  auto connection = DPLobby::get()->getConnectionSettings();
  if (connection == NULL) {
    g_critical("[getSessionGuid] DPLConnection is null");
  }
  auto guid = connection->getSessionDesc()->getSessionGuid();
  delete connection;
  return guid;
}

static SignalingConnection* enumSessionsSC;

static HRESULT WINAPI DPNice_EnumSessions (DPSP_ENUMSESSIONSDATA* data) {
  g_debug(
    "EnumSessions (%p,%ld,%p,%u) stub",
    data->lpMessage, data->dwMessageSize,
    data->lpISP, data->bReturnStatus
  );

  cout << data << endl;

  auto provider = data->lpISP;

  auto guid = getSessionGuid();
  g_debug("[EnumSessions] guidInstance = %s", to_string(guid).c_str());

  if (!enumSessionsSC) {
    enumSessionsSC = new SignalingConnection(
      DEFAULT_SIGNALING_HOST, DEFAULT_SIGNALING_PORT);
    enumSessionsSC->onEnumSessionsResponse = [provider] (auto data, auto size) {
      g_debug("[EnumSessions] Discovered session %d", size);
      provider->HandleMessage(data, size, 0);
    };
  }
  enumSessionsSC->relayEnumSessions(getSessionGuid());

  return DP_OK;
}

// FIXME This is used to tell the signaling server which EnumSessions message we
// are attempting to reply to. Pretty hacky, perhaps store this "reply-to
// number" in the message header for Name Server messages instead?
static int currentEnumSessionsMessageId = 0;

static HRESULT WINAPI DPNice_Reply (DPSP_REPLYDATA* data) {
  g_debug(
    "Reply (%p,%p,%ld,%ld,%p) stub",
    data->lpSPMessageHeader, data->lpMessage, data->dwMessageSize,
    data->idNameServer, data->lpISP
  );

  cout << data << endl;

  auto session = getGameSession(data->lpISP);
  g_debug("[DPNice_Reply] currentEnumSessionsMessageId = %d", currentEnumSessionsMessageId);
  // LOL. Careful! Assuming here that this is indeed an enumsessions reply message.
  // FIXME Really actually only reply to enumsessions messages here. Perhaps by
  // parsing the message envelope to check.
  if (currentEnumSessionsMessageId != 0) {
    g_debug("[DPNice_Reply] SignalingConnection (%p)", session->getSignalingConnection());
    session->getSignalingConnection()->relayEnumSessionsResponse(
      currentEnumSessionsMessageId,
      data->lpMessage,
      data->dwMessageSize
    );
    currentEnumSessionsMessageId = 0;
    return DP_OK;
  }

  DPID target = 0;
  try {
    target = _replyTargetsHACK.at(data->lpMessage);
  } catch (std::out_of_range) {
    g_warning("[DPNice_Reply] Not sure what to reply to");
    return DPERR_UNSUPPORTED;
  }

  g_debug("[DPNice_Reply] Replying to %ld", target);
  auto player = session->getPlayerById(target);
  if (!player) {
    g_warning("[DPNice_Reply] Could not find player");
    return DPERR_UNAVAILABLE;
  }

  player->send(data->dwMessageSize, data->lpMessage);
  return DP_OK;
}

static HRESULT WINAPI DPNice_Send (DPSP_SENDDATA* data) {
  g_debug(
    "Send (0x%08lx,%ld,%ld,%p,%ld,%u,%p)",
    data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpMessage, data->dwMessageSize,
    data->bSystemMessage, data->lpISP
  );

  cout << data << endl;

  auto provider = data->lpISP;
  auto session = getGameSession(provider);

  auto target = data->idPlayerTo;
  Player* player;
  if (target == 0) {
    g_debug("[DPNice_Send] Send to name server");
    player = session->getNameServerPlayer();
  } else {
    g_debug("[DPNice_Send] Send to player %ld", target);
    player = session->getPlayerById(target);
  }

  if (!player) {
    g_warning("[DPNice_Send] Player not found");
    return DPERR_UNAVAILABLE;
  }

  g_debug("[DPNice_Send] player = %ld", player->id);

  player->send(data->dwMessageSize, data->lpMessage);

  return DP_OK;
}

static HRESULT WINAPI DPNice_CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  g_debug(
    "CreatePlayer (%ld,0x%08lx,%p,%p)",
    data->idPlayer, data->dwFlags,
    data->lpSPMessageHeader, data->lpISP
  );

  cout << data << endl;

  auto provider = data->lpISP;
  auto session = getGameSession(provider);

  const auto isLocal = (data->dwFlags & DPLAYI_PLAYER_PLAYERLOCAL) != 0;
  const auto isNameServer = (data->dwFlags & DPLAYI_PLAYER_NAMESRVR) != 0;
  const auto isSystemPlayer = (data->dwFlags & DPLAYI_PLAYER_SYSPLAYER) != 0;

  if (isLocal && !isSystemPlayer) {
    // We are player $data->idPlayer
    session->setLocalPlayer(data->idPlayer);
  }
  if (isNameServer) {
    session->setNameServer(data->idPlayer);
    if (isLocal) {
      // We are the name server
      // TODO Do something here?
      // session->getSignalingConnection()
      //   ->authenticate(data->idPlayer);
    }
  }

  const auto player = session->getPlayerById(data->idPlayer);
  if (player) {
    auto refPlayer = &player;
    provider->SetSPPlayerData(data->idPlayer, &refPlayer, sizeof(refPlayer), DPSET_LOCAL);
  }

  return DP_OK;
}

static HRESULT WINAPI DPNice_DeletePlayer (DPSP_DELETEPLAYERDATA* data) {
  g_debug(
    "DeletePlayer (%ld,0x%08lx,%p)",
    data->idPlayer, data->dwFlags, data->lpISP
  );

  cout << data << endl;

  auto session = getGameSession(data->lpISP);

  session->deletePlayer(data->idPlayer);

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddress (DPSP_GETADDRESSDATA* data) {
  g_debug(
    "GetAddress (%ld,0x%08lx,%p,%p,%p) stub",
    data->idPlayer, data->dwFlags, data->lpAddress,
    data->lpdwAddressSize, data->lpISP
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetCaps (DPSP_GETCAPSDATA* data) {
  g_debug(
    "GetCaps (%ld,%p,0x%08lx,%p)",
    data->idPlayer, data->lpCaps, data->dwFlags, data->lpISP
  );

  cout << data << endl;

  data->lpCaps->dwFlags = 0;
  data->lpCaps->dwMaxBufferSize = 1024;
  data->lpCaps->dwMaxQueueSize = 0;
  data->lpCaps->dwMaxPlayers = 65536;
  data->lpCaps->dwHundredBaud = 0;
  data->lpCaps->dwLatency = 50;
  data->lpCaps->dwMaxLocalPlayers = 65536;
  data->lpCaps->dwHeaderLength = 0;
  data->lpCaps->dwTimeout = 500;

  return DP_OK;
}

static void* startThread (void*) {
  CoInitialize(NULL);

  g_debug("[GMainLoop::startThread] loop start");
  g_main_loop_run(gloop);
  g_debug("[GMainLoop::startThread] loop complete");
  g_main_loop_unref(gloop);

  CoUninitialize();
  g_thread_exit(NULL);
  return nullptr;
}

static HRESULT WINAPI DPNice_Open (DPSP_OPENDATA* data) {
  g_debug(
    "Open (%u,%p,%p,%u,0x%08lx,0x%08lx)",
    data->bCreate, data->lpSPMessageHeader, data->lpISP,
    data->bReturnStatus, data->dwOpenFlags, data->dwSessionFlags
  );

  cout << data << endl;

  const auto isHost = !!data->bCreate;
  auto provider = data->lpISP;
  _providerHACK = provider;

  g_thread_new("main loop", &startThread, NULL);

  if (enumSessionsSC) {
    delete enumSessionsSC;
    enumSessionsSC = NULL;
  }

  g_debug("[Open] new session (provider = %p)", provider);
  const auto host = DEFAULT_SIGNALING_HOST;
  const auto port = DEFAULT_SIGNALING_PORT;
  auto connection = new SignalingConnection(host, port);
  auto session = new GameSession(connection, isHost);

  g_debug("[Open] SetSPData (stubbed)");
  _gameSession = session;

  // Register immediately, with fake IDs for now.
  // TODO Initially connect with zeroed IDs, then update them in CreatePlayer.
  connection->connect(getSessionGuid(), isHost);

  session->onMessage = [] (const auto id, auto data, const auto size) {
    g_debug("[Open/onMessage] Receiving message (%d bytes) from %ld", size, id);
    _replyTargetsHACK[data] = id;

    g_debug("[Open/onMessage] probe provider (%p)", _providerHACK);
    DWORD flags = 0;
    auto hr = _providerHACK->GetPlayerFlags(0, &flags);
    g_debug("[Open/onMessage] result = %02lx", hr);

    g_debug("[Open/onMessage] HandleMessage (cmd = %02x)", reinterpret_cast<short*>(data)[2]);
    _providerHACK->HandleMessage(data, size, NULL);
    g_debug("[Open/onMessage] Handled message");
    _replyTargetsHACK.erase(data);
  };

  if (isHost) {
    connection->onEnumSessions = [provider] (const auto id) {
      g_debug("[Open] onEnumSessions %d", id);
      // The signaling server asked us to send info about local sessions, so
      // create a fake ENUMSESSIONS message for DirectPlay
      auto message = new DPSP_MSG_ENUMSESSIONS;
      message->flags = 0;
      message->passwordOffset = 0;
      message->applicationGuid = DPLobby::get()
        ->getConnectionSettings()
        ->getSessionDesc()
        ->getApplicationGuid();
      currentEnumSessionsMessageId = id;
      provider->HandleMessage(message, sizeof(*message), NULL);
    };
  } else {
    session->waitUntilConnectedWithHost();
    g_debug("[Open] Connected w/Host (provider = %p)", provider);
  }

  return DP_OK;
}

static HRESULT WINAPI DPNice_CloseEx (DPSP_CLOSEDATA* data) {
  g_debug("CloseEx (%p) stub", data->lpISP);

  cout << data << endl;

  auto session = getGameSession(data->lpISP);

  g_main_loop_quit(gloop);
  delete session;

  return DP_OK;
}

static HRESULT WINAPI DPNice_ShutdownEx (DPSP_SHUTDOWNDATA* data) {
  g_debug("ShutdownEx (%p) stub", data->lpISP);

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddressChoices (DPSP_GETADDRESSCHOICESDATA* data) {
  g_debug(
    "GetAddressChoices (%p,%p,%p) stub",
    data->lpAddress, data->lpdwAddressSize, data->lpISP
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendEx (DPSP_SENDEXDATA* data) {
  g_debug(
    "SendEx (%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p,%u) stub",
    data->lpISP, data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID, data->bSystemMessage
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendToGroupEx (DPSP_SENDTOGROUPEXDATA* data) {
  g_debug(
    "SendToGroupEx (%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p) stub",
    data->lpISP, data->dwFlags, data->idGroupTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Cancel (DPSP_CANCELDATA* data) {
  g_debug(
    "Cancel (%p,0x%08lx,%p,%ld,0x%08lx,0x%08lx) stub",
    data->lpISP, data->dwFlags, data->lprglpvSPMsgID, data->cSPMsgID,
    data->dwMinPriority, data->dwMaxPriority
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetMessageQueue (DPSP_GETMESSAGEQUEUEDATA* data) {
  g_debug(
    "GetMessageQueue (%p,0x%08lx,%ld,%ld,%p,%p) stub",
    data->lpISP, data->dwFlags, data->idFrom, data->idTo,
    data->lpdwNumMsgs, data->lpdwNumBytes
  );

  cout << data << endl;

  return DPERR_UNSUPPORTED;
}

static void setupCallbacks (DPSP_SPCALLBACKS* callbacks) {
  callbacks->EnumSessions = DPNice_EnumSessions;
  callbacks->Reply = DPNice_Reply;
  callbacks->Send = DPNice_Send;
  callbacks->CreatePlayer = DPNice_CreatePlayer;
  callbacks->DeletePlayer = DPNice_DeletePlayer;
  callbacks->GetAddress = DPNice_GetAddress;
  callbacks->GetCaps = DPNice_GetCaps;
  callbacks->Open = DPNice_Open;
  callbacks->CloseEx = DPNice_CloseEx;
  callbacks->ShutdownEx = DPNice_ShutdownEx;
  callbacks->GetAddressChoices = DPNice_GetAddressChoices;
  callbacks->SendEx = DPNice_SendEx;
  callbacks->SendToGroupEx = DPNice_SendToGroupEx;
  callbacks->Cancel = DPNice_Cancel;
  callbacks->GetMessageQueue = DPNice_GetMessageQueue;

  callbacks->AddPlayerToGroup = NULL;
  callbacks->Close = NULL;
  callbacks->CreateGroup = NULL;
  callbacks->DeleteGroup = NULL;
  callbacks->RemovePlayerFromGroup = NULL;
  callbacks->SendToGroup = NULL;
  callbacks->Shutdown = NULL;
}

static HRESULT init (SPINITDATA* spData) {
  g_debug("SPInit");

  g_info(
    "[SPInit] Initializing library for %s (%ls)",
    to_string(*spData->lpGuid).c_str(),
    spData->lpszName
  );

  // We only support NICE service
  if (!IsEqualGUID(*spData->lpGuid, DPSPGUID_NICE)) {
    return DPERR_UNAVAILABLE;
  }

  setupCallbacks(spData->lpCB);

  g_debug("[SPInit] Have callbacks");
  g_debug("[SPInit] Provider = %p", spData->lpISP);

  gloop = g_main_loop_new(NULL, FALSE);

  /* dplay needs to know the size of the header */
  spData->dwSPHeaderSize = 0;
  spData->dwSPVersion = DPSP_MAJORVERSIONMASK & DPSP_MAJORVERSION;

  return DP_OK;
}

}

__declspec(dllexport) HRESULT __cdecl SPInit (SPINITDATA* spData) {
  return nicesp::init(spData);
}

G_LOCK_DEFINE(loglock);
static void logToFile (const gchar* domain, GLogLevelFlags, const gchar* message, void* ctx) {
  auto output = static_cast<ofstream*>(ctx);
  G_LOCK(loglock);
  if (domain != NULL) {
    *output << domain << "  " << message << endl;
  }
  if (message != NULL) {
    *output << "glog " << message << endl;
  }
  G_UNLOCK(loglock);
}

streambuf* coutbuf;
ofstream logfile;
static void setupLogging () {
  logfile.open("C:\\nicesp-" + to_string(time(0)) + ".txt");
  // Send g_log messages to file
  g_log_set_default_handler(logToFile, &logfile);
  // Redirect cout to log file
  coutbuf = cout.rdbuf();
  cout.rdbuf(logfile.rdbuf());
}
static void teardownLogging () {
  cout.rdbuf(coutbuf);
  logfile.close();
}

BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, void* reserved) {
  g_debug("DllMain (0x%p, %ld, %p)", instance, reason, reserved);

  switch (reason) {
  case DLL_PROCESS_ATTACH:
    setupLogging();

    g_message("--------------------");
    g_message("[DllMain] INITIALISE");
    g_message("--------------------");

    DisableThreadLibraryCalls(instance);
    break;
  case DLL_PROCESS_DETACH:
    g_message("-----------------");
    g_message("[DllMain] RELEASE");
    g_message("-----------------");

    teardownLogging();
    break;
  default:
    break;
  }
  return TRUE;
}
