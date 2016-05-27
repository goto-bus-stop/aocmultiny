#include "common.hpp"
#include "dplib/main.hpp"

namespace aocmultiny {

class Lobby {
private:
  IDirectPlay3* dp;
  LPDIRECTPLAYLOBBY3A dpLobby = NULL;

  HRESULT create ();
  void launch (dplib::DPAddress address, bool isHosting);
  bool receiveMessage (DWORD appId);

public:
  Lobby ();

  void host ();
  void join (std::string remoteIp);
};

}
