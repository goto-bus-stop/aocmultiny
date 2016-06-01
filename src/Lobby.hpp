#include "common.hpp"
#include "dplib/main.hpp"

namespace aocmultiny {

class Lobby {
private:
  IDirectPlay3* dp;
  LPDIRECTPLAYLOBBY3A dpLobby = NULL;
  GUID guid;
  bool isHosting;
  std::string hostIp;

  HRESULT create ();
  bool receiveMessage (DWORD appId);

public:
  Lobby ();

  void host ();
  void join (GUID sessionId, std::string hostIp);
  GUID getSessionGUID ();

  void launch ();
  EventListeners<> onConnectSucceeded;
  EventListeners<> onAppTerminated;
};

}
