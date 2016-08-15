#include "GameSession.hpp"
#include <stdio.h>

namespace nicesp {

static void onNiceCandidates (InternalAgent, guint streamId, gpointer data) {
  g_message("[onNiceCandidates] SIGNAL candidate gathering done\n");
  auto player = reinterpret_cast<Player*>(data);
  auto sdp = player->agent->generateLocalSdp();

  auto signaling = player->getSession()->getSignalingConnection();
  signaling->sendSdp(player->id, sdp);

  g_message("\n[onNiceCandidates] SDP\n===\n");
  g_message("%s\n===\n\n", sdp);
}

static void onNiceStateChange (
  InternalAgent,
  guint streamId,
  guint componentId,
  guint state,
  gpointer data
) {
  g_message("[onNiceStateChange] SIGNAL statechange %d %d %d\n", streamId, componentId, state);
  auto player = reinterpret_cast<Player*>(data);
  if (state == NICE_COMPONENT_STATE_READY) {
    g_message("[onNiceStateChange] Ready %d\n", static_cast<int>(player->id));
    player->agent->send(streamId, componentId, 3, "hoi");
  }
}

static void onReceiveNicePacket (
  InternalAgent,
  guint streamId, guint componentId,
  guint len, gchar* buf,
  gpointer data
) {
  auto player = reinterpret_cast<Player*>(data);
  g_message("[onReceiveNicePacket] received %s from %d\n", buf, player->id);
}

Player::Player (GameSession* session, DPID id)
    :
    session(session),
    id(id),
    agent(new NiceAgent(g_main_loop_get_context(gloop))) {
  // Set the STUN settings and controlling mode
  g_object_set(this->agent->unwrap(), "stun-server", "stun.l.google.com", NULL);
  g_object_set(this->agent->unwrap(), "stun-server-port", 19302, NULL);

  // Connect to the signals
  g_signal_connect(this->agent->unwrap(), "candidate-gathering-done", G_CALLBACK(onNiceCandidates), this);
  g_signal_connect(this->agent->unwrap(), "component-state-changed", G_CALLBACK(onNiceStateChange), this);
}

Player::~Player () {
  g_object_unref(this->agent);
}

GameSession* Player::getSession () {
  return this->session;
}

GameSession::GameSession (SignalingConnection* signaling, bool isHost)
    :
    players(vector<Player*>()),
    signaling(signaling),
    isHost(isHost) {
  signaling->onNewPlayer = [this] (auto id) {
    this->processNewPlayer(id);
  };
  signaling->onSdp = [this] (auto id, auto sdp) {
    this->processSdp(id, sdp);
  };
}

GameSession::~GameSession () {
  // Nothing yet
}

NiceAgent* GameSession::getPlayerAgent (DPID id) {
  return this->getPlayerById(id)->agent;
}

SignalingConnection* GameSession::getSignalingConnection () {
  return this->signaling;
}

void GameSession::processNewPlayer (DPID id) {
  auto player = new Player(this, id);
  g_object_set(player->agent->unwrap(), "controlling-mode", this->isHost ? 1 : 0, NULL);

  auto stream = player->agent->addStream(1);
  stream->attachRecv(1, g_main_loop_get_context(gloop), onReceiveNicePacket, player);

  this->players.push_back(player);

  stream->gatherCandidates();
}

void GameSession::deletePlayer (DPID id) {
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
  gsize sdpLen;
  auto rawSdp = reinterpret_cast<gchar*>(g_base64_decode(sdp, &sdpLen));
  auto player = this->getPlayerById(id);
  player->agent->parseRemoteSdp(rawSdp);
}

Player* GameSession::getPlayerById (DPID id) {
  for (auto p : this->players) {
    if (p->id == id) {
      return p;
    }
  }
  return nullptr;
}

}
