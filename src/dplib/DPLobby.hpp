#include "DPAddress.hpp"
#include "DPLConnection.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"
#include "util.hpp"
#include "../util.hpp"
#include <dplay.h>
#include <dplobby.h>

using aocmultiny::EventListeners;

namespace aocmultiny {
namespace dplib {

class DPGame;
class DPLobby;
class DPLobbyMessage;

class DPLobbyMessage {
private:
  bool shouldStop;
public:
  int appId;
  DPLobby* lobby;
  int flags;
  int size;
  void* data;

  DPLobbyMessage (DPLobby* lobby, int appId, int flags, void* data, int size);
  ~DPLobbyMessage ();
  void reply (void* data, int size);
  void stop ();
  bool requestedStop ();

  template<typename T>
  T as ();
};

class DPLobby {
private:
  IDirectPlay3* dp;
  LPDIRECTPLAYLOBBY3A dpLobby = NULL;

  DPGame* game;

  GUID guid;
  bool isHosting;
  std::string hostIp;
  std::string playerName;

  HRESULT create ();
  bool receiveMessage (DWORD appId);

public:
  DPLobby (DPGame* game, std::string playerName);
  DPLobby (DPGame* game);

  void host ();
  void join (GUID sessionId, std::string hostIp);
  GUID getSessionGUID ();

  void sendLobbyMessage (int flags, int appId, void* data, int size);

  void launch ();
  EventListeners<> onConnectSucceeded;
  EventListeners<> onAppTerminated;
};

}
}
