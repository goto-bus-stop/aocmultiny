#pragma once
#include "NiceAgent.hpp"
#include "SignalingConnection.hpp"
#include <dplay.h>
#include <vector>

using std::vector;

namespace nicesp {

extern GMainLoop* gloop;

class GameSession;

class Player {
private:
  GameSession* session;
public:
  DPID id;
  NiceAgent* agent;

  Player (GameSession* session, DPID id);
  ~Player ();
  GameSession* getSession ();
};

class GameSession {
private:
  vector<Player*> players;
  bool isHost;
  SignalingConnection* signaling;
public:

  GameSession (SignalingConnection* signaling, bool isHost);
  ~GameSession ();

  NiceAgent* getPlayerAgent (DPID id);
  SignalingConnection* getSignalingConnection ();

  void processNewPlayer (DPID id);
  void deletePlayer (DPID id);
  void processSdp (DPID id, const gchar* sdp);
  Player* getPlayerById (DPID id);
};

}
