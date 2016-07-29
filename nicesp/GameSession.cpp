#include "GameSession.hpp"
#include <stdio.h>

namespace nicesp {

static void onNiceCandidates (InternalAgent, guint streamId, gpointer data) {
  printf("SIGNAL candidate gathering done\n");
  auto player = reinterpret_cast<Player*>(data);
  auto sdp = player->agent->generateLocalSdp();

  auto sdp64 = g_base64_encode((const guchar*) sdp, strlen(sdp));
  auto sdpString = g_strdup_printf("sdp to:%d,sdp:%s\n", player->id, sdp64);
  player->getSession()->sendSignalingMessage(sdpString);
  g_free(sdp64);
  g_free(sdpString);

  printf("\nSDP\n===\n");
  printf("%s\n===\n\n", sdp);
}

static void onNiceStateChange (
  InternalAgent,
  guint streamId,
  guint componentId,
  guint state,
  gpointer data
) {
  printf("SIGNAL statechange %d %d %d\n", streamId, componentId, state);
  auto player = reinterpret_cast<Player*>(data);
  if (state == NICE_COMPONENT_STATE_READY) {
    printf("Ready %d\n", player->id);
    player->agent->send(streamId, componentId, 3, "hoi");
  }
}

static void onReceiveNicePacket (
  InternalAgent agent,
  guint streamId, guint componentId,
  guint len, gchar* buf,
  gpointer data
) {
  auto player = reinterpret_cast<Player*>(data);
  printf("received %s from %d\n", buf, player->id);
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

GameSession* Player::getSession () {
  return this->session;
}

GameSession::GameSession (GUID sessionGuid, bool isHost)
    :
    sessionGuid(sessionGuid),
    isHost(isHost),
    players(vector<Player*>()) {
}

GameSession::~GameSession () {
  // Nothing yet
}

NiceAgent* GameSession::getPlayerAgent (DPID id) {
  return this->getPlayerById(id)->agent;
}

GUID GameSession::getSessionGuid () {
  return this->sessionGuid;
}

void GameSession::useSignalingIOStreams (GDataInputStream* input, GOutputStream* output) {
  this->receiveStream = input;
  this->sendStream = output;
}

void GameSession::sendSignalingMessage (gchar* message) {
  g_output_stream_write(this->sendStream, message, strlen(message), NULL, NULL);
}

void GameSession::processNewPlayer (DPID id) {
  auto player = new Player(this, id);
  g_object_set(player->agent->unwrap(), "controlling-mode", this->isHost ? 1 : 0, NULL);

  auto stream = player->agent->addStream(1);
  stream->attachRecv(1, g_main_loop_get_context(gloop), onReceiveNicePacket, player);

  this->players.push_back(player);

  stream->gatherCandidates();
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
