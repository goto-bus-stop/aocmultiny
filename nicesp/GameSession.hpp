#pragma once
#include "NiceAgent.hpp"
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
  GameSession* getSession ();
};

class GameSession {
private:
  GUID sessionGuid;
  vector<Player*> players;
  bool isHost;

  GDataInputStream* receiveStream;
  GOutputStream* sendStream;
public:
  GameSession (GUID sessionGuid, bool isHost);
  ~GameSession ();

  NiceAgent* getPlayerAgent (DPID id);
  GUID getSessionGuid ();

  void useSignalingIOStreams (GDataInputStream* input, GOutputStream* output);
  void sendSignalingMessage (gchar* message);

  void processNewPlayer (DPID id);
  void processSdp (DPID id, const gchar* sdp);
  Player* getPlayerById (DPID id);
};

}
