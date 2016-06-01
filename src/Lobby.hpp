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
  std::string playerName;

  HRESULT create ();
  bool receiveMessage (DWORD appId);

public:
  Lobby (std::string playerName);
  Lobby ();

  void host ();
  void join (GUID sessionId, std::string hostIp);
  GUID getSessionGUID ();

  void launch ();
  EventListeners<> onConnectSucceeded;
  EventListeners<> onAppTerminated;
};

}
