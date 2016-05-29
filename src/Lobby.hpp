#include "common.hpp"
#include "dplib/main.hpp"

namespace aocmultiny {

class Lobby {
private:
  IDirectPlay3* dp;
  LPDIRECTPLAYLOBBY3A dpLobby = NULL;
  GUID guid;
  bool isHosting;

  HRESULT create ();
  void launch (dplib::DPAddress address);
  bool receiveMessage (DWORD appId);

public:
  Lobby ();

  void host ();
  void join (GUID gameId, std::string remoteIp);
};

}
