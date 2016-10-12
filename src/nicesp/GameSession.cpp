#include "GameSession.hpp"
#include <stdio.h>

namespace nicesp {

static void onNiceCandidates (InternalAgent, guint streamId, gpointer data) {
  g_debug("[onNiceCandidates] SIGNAL candidate gathering done");
  auto player = reinterpret_cast<RemotePlayer*>(data);
  auto sdp = player->agent->generateLocalSdp();

  auto signaling = player->getSession()->getSignalingConnection();
  signaling->sendSdp(player->id, sdp);

  g_debug("\n[onNiceCandidates] SDP\n===\n");
  g_debug("%s\n===\n\n", sdp);
}

static void onNiceStateChange (
  InternalAgent,
  guint streamId,
  guint componentId,
  guint state,
  gpointer data
) {
  g_debug("[onNiceStateChange] SIGNAL statechange %d %d %d", streamId, componentId, state);
  auto player = reinterpret_cast<Player*>(data);
  if (state == NICE_COMPONENT_STATE_READY) {
    g_debug("[onNiceStateChange] Ready %d", static_cast<int>(player->id));
    // player->agent->send(streamId, componentId, 3, "hoi");
    player->getSession()->processReady(player->id);
  }
}

static void onReceiveNicePacket (
  InternalAgent,
  guint streamId, guint componentId,
  guint len, gchar* buf,
  gpointer data
) {
  g_debug("[onReceiveNicePacket] received %d [%p]", len, buf);
  auto player = reinterpret_cast<Player*>(data);
  g_debug("[onReceiveNicePacket] from %ld", player->id);
  auto onMessage = player->getSession()->onMessage;
  onMessage(buf, len);
}

Player::Player (GameSession* session, DPID id)
    :
    session(session),
    id(id) {
  g_debug("[Player::Player]");
}

Player::~Player () {
  g_debug("[Player::~Player]");
}

GameSession* Player::getSession () {
  g_debug("[Player::getSession]");
  return this->session;
}

gint Player::send (gsize size, void* message) {
  g_debug("[Player::send] should not be called!");
  return 0;
}

RemotePlayer::RemotePlayer (GameSession* session, DPID id)
    :
    Player(session, id),
    agent(new NiceAgent(g_main_loop_get_context(gloop))) {
  g_debug("[RemotePlayer::RemotePlayer]");
  // Set the STUN settings and controlling mode
  g_object_set(this->agent->unwrap(), "stun-server", "74.125.136.127", NULL);
  g_object_set(this->agent->unwrap(), "stun-server-port", 19302, NULL);

  // Connect to the signals
  g_signal_connect(this->agent->unwrap(), "candidate-gathering-done", G_CALLBACK(onNiceCandidates), this);
  g_signal_connect(this->agent->unwrap(), "component-state-changed", G_CALLBACK(onNiceStateChange), this);
}

RemotePlayer::~RemotePlayer () {
  g_debug("[RemotePlayer::~RemotePlayer]");
  g_object_unref(this->agent);
}

gint RemotePlayer::send (gsize size, void* message) {
  g_debug("[RemotePlayer::send] Sending %d bytes", size);
  auto stream = this->agent->getStream(1);
  return stream->send(1, size, static_cast<gchar*>(message));
}

LocalPlayer::LocalPlayer (GameSession* session, DPID id)
    :
    Player(session, id) {
  g_debug("[LocalPlayer::LocalPlayer]");
}

LocalPlayer::~LocalPlayer () {
  g_debug("[LocalPlayer::~LocalPlayer]");
}

gint LocalPlayer::send (gsize size, void* message) {
  g_debug("[LocalPlayer::send] Processing %d bytes locally", size);
  auto onMessage = this->getSession()->onMessage;
  onMessage(this->id, message, size);
  return 0;
}

GameSession::GameSession (SignalingConnection* signaling, bool isHost)
    :
    players(vector<RemotePlayer*>()),
    signaling(signaling),
    nameServer(nullptr),
    localPlayer(nullptr),
    isHost(isHost) {
  g_debug("[GameSession] new %d", isHost);
  signaling->onNewPlayer = [this] (auto id) {
    this->processNewPlayer(id);
  };
  signaling->onSdp = [this] (auto id, auto sdp) {
    this->processSdp(id, sdp);
  };
}

GameSession::~GameSession () {
  g_debug("[GameSession::~GameSession] destroy");
  // Nothing yet
}

void GameSession::setLocalPlayer (DPID id) {
  g_debug("[GameSession::setLocalPlayer] %ld", id);
  if (this->localPlayer != nullptr) {
    delete this->localPlayer;
  }
  this->localPlayer = new LocalPlayer(this, id);
}

void GameSession::setNameServer (DPID id) {
  g_debug("[GameSession::setNameServer] %ld", id);
  if (this->nameServer != nullptr) {
    delete this->nameServer;
  }
  this->nameServer = this->getPlayerById(id);
  if (!this->nameServer) {
    g_debug("[GameSession::setNameServer] local");
    this->nameServer = new LocalPlayer(this, id);
  }
}

SignalingConnection* GameSession::getSignalingConnection () {
  g_debug("[GameSession::getSignalingConnection]");
  return this->signaling;
}

void GameSession::processNewPlayer (DPID id) {
  g_debug("[GameSession::processNewPlayer] %ld", id);
  auto player = new RemotePlayer(this, id);
  g_object_set(player->agent->unwrap(), "controlling-mode", this->isHost ? 1 : 0, NULL);

  auto stream = player->agent->addStream(1);
  stream->setName("application");
  stream->attachRecv(1, g_main_loop_get_context(gloop), onReceiveNicePacket, player);

  this->players.push_back(player);
  // The game host is always the name server for now.
  if (!this->isHost && this->players.size() == 1) {
    this->setNameServer(id);
  }

  stream->gatherCandidates();
}

void GameSession::deletePlayer (DPID id) {
  g_debug("[GameSession::deletePlayer] %ld", id);
  for (int i = 0, l = this->players.size(); i < l; i++) {
    auto player = this->players[i];
    if (player->id == id) {
      this->players.erase(this->players.begin() + i);
      delete player;
      return;
    }
  }
}

void GameSession::processSdp (DPID id, const gchar* sdp) {
  g_debug("[GameSession::processSdp] %ld", id);
  gsize sdpLen;
  auto rawSdp = reinterpret_cast<gchar*>(g_base64_decode(sdp, &sdpLen));
  for (auto player : this->players) {
    if (player->id == id) {
      player->agent->parseRemoteSdp(rawSdp);
    }
  }
}

void GameSession::processReady (DPID remote) {
  g_debug("[GameSession::processReady] %ld", remote);
  if (!this->isHost) {
    if (this->remoteSdpEvent) {
      SetEvent(this->remoteSdpEvent);
    }
  }
}

void GameSession::waitUntilConnectedWithHost () {
  g_debug("[GameSession::waitUntilConnectedWithHost]");
  if (!this->isHost) {
    this->remoteSdpEvent = CreateEvent(NULL, false, false, NULL);
    WaitForSingleObject(this->remoteSdpEvent, 30000);
    this->remoteSdpEvent = NULL;
  }
}

Player* GameSession::getPlayerById (DPID id) {
  g_debug("[GameSession::getPlayerById] known players:");
  if (this->localPlayer) {
    g_debug("   local = %ld", this->localPlayer->id);
  }
  if (this->nameServer) {
    g_debug("   ns = %ld", this->nameServer->id);
  }
  for (auto p : this->players) {
    g_debug("       %ld", p->id);
  }

  if (this->localPlayer && this->localPlayer->id == id) {
    return this->localPlayer;
  }
  if (this->nameServer && this->nameServer->id == id) {
    return this->nameServer;
  }
  for (auto p : this->players) {
    if (p->id == id) {
      return p;
    }
  }
  return nullptr;
}

Player* GameSession::getLocalPlayer () {
  g_debug("[GameSession::getLocalPlayer] %p", this->localPlayer);
  return this->localPlayer;
}

Player* GameSession::getNameServerPlayer () {
  g_debug("[GameSession::getNameServerPlayer] %p", this->nameServer);
  return this->nameServer;
}

}
