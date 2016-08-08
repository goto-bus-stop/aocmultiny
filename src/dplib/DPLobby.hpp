#pragma once
#include "DPAddress.hpp"
#include "DPGame.hpp"
#include "DPSession.hpp"
#include "util.hpp"
#include <dplay.h>
#include <dplobby.h>

using std::string;

namespace dplib {

class DPSession;

class DPLobbyMessage {
private:
  bool shouldStop;
public:
  int appId;
  DPSession* session;
  int flags;
  int size;
  void* data;

  DPLobbyMessage (int appId, int flags, void* data, int size);
  ~DPLobbyMessage ();
  void reply (void* data, int size);
  void stop ();
  bool requestedStop ();

  template<typename T>
  T as ();
};

class DPLobby {
private:
  IDirectPlay4A* dp;
  IDirectPlayLobby3A* dpLobby = NULL;

  HRESULT create ();

  DPLobby ();

public:
  static DPLobby* get ();

  IDirectPlayLobby3A* getInternalLobby ();

  DPLConnection* getConnectionSettings (DWORD appId = 0);
  HRESULT setConnectionSettings (DWORD appId, DPLConnection* connection);

  DPSession* createSession (DPGame* game);
  DPSession* hostSession (DPGame* game);
  DPSession* hostSession (DPGame* game, DPAddress* address);
  DPSession* joinSession (DPGame* game, DPAddress* address, GUID sessionGuid);

  DPLobbyMessage* receiveLobbyMessage (DWORD appId);
  void sendLobbyMessage (int flags, int appId, void* data, int size);
};

}
