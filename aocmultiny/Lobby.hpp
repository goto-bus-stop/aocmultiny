#include "common.hpp"

namespace aocmultiny {

class Lobby {
private:
  IDirectPlay3* dp;
  LPDIRECTPLAYLOBBY3A dpLobby = NULL;

  HRESULT create ();

public:
  Lobby ();

  void host ();
};

}
