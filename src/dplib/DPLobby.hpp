#include "../common.hpp"
#include <dplay.h>
#include <dplobby.h>
#include "DPAddress.hpp"
#include "DPGame.hpp"
#include "DPLConnection.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"
#include "util.hpp"

using aocmultiny::EventListeners;

namespace aocmultiny {
namespace dplib {

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

  void launch ();
  EventListeners<> onConnectSucceeded;
  EventListeners<> onAppTerminated;
};

}
}
