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
protected:
  GameSession* session;
public:
  DPID id;

  Player (GameSession* session, DPID id);
  virtual ~Player ();
  GameSession* getSession ();
  virtual gint send (gsize size, void* message);
};

class RemotePlayer: public Player {
public:
  NiceAgent* agent;
  RemotePlayer (GameSession* session, DPID id);
  virtual ~RemotePlayer ();

  virtual gint send(gsize size, void* message);
};

class LocalPlayer: public Player {
public:
  LocalPlayer (GameSession* session, DPID id);
  virtual ~LocalPlayer ();

  virtual gint send(gsize size, void* message);
};

class GameSession {
private:
  vector<RemotePlayer*> players;
  SignalingConnection* signaling;
  HANDLE remoteSdpEvent = NULL;
  Player* nameServer;
  Player* localPlayer;
public:
  GameSession (SignalingConnection* signaling, bool isHost);
  ~GameSession ();

  bool isHost;

  SignalingConnection* getSignalingConnection ();

  void setLocalPlayer (DPID id);
  void setNameServer (DPID id);

  Player* getPlayerById (DPID id);
  Player* getLocalPlayer ();
  Player* getNameServerPlayer ();

  void processNewPlayer (DPID id);
  void deletePlayer (DPID id);
  void processSdp (DPID id, const gchar* sdp);
  void processReady (DPID remote);

  void waitUntilConnectedWithHost ();

  function<void(void*, gsize)> onMessage;
};

}
