#pragma once
#include "DPAddress.hpp"
#include "DPLConnection.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"
#include "DPLobby.hpp"
#include "util.hpp"
#include "../util.hpp"
#include <dplay.h>
#include <dplobby.h>
#include <string>

using std::string;
using aocmultiny::EventListeners;

namespace dplib {

class DPLobby;

class DPSession {
private:
  DPLobby* lobby;
  DPGame* game;

  GUID guid;
  bool isHosting;
  DPAddress* address;

  string playerName;

  bool receiveMessage (DWORD appId);

public:
  DPSession (DPGame* game, string playerName);
  DPSession (DPGame* game);

  DPSession* setPlayerName (string playerName);

  DPSession* host (DPAddress* address);
  DPSession* join (DPAddress* address, GUID sessionId);
  GUID getSessionGUID ();

  void sendLobbyMessage (int flags, int appId, void* data, int size);

  void launch ();
  EventListeners<> onConnectSucceeded;
  EventListeners<> onAppTerminated;
};

}
