#pragma once
#include <dplobby.h>

#define DEFINE_DPGAME(name, guid) \
  class name: public aocmultiny::dplib::DPGame { \
  public: \
    name () : DPGame(guid) {}; \
    GUID getGameGuid (); \
    void receiveMessage (aocmultiny::dplib::DPLobbyMessage* message); \
  }

namespace aocmultiny {
namespace dplib {

class DPLobbyMessage;

class DPGame {
private:
  GUID guid;
public:
  DPGame (GUID gameGuid);

  GUID getGameGuid ();
  virtual void receiveMessage (DPLobbyMessage* message);
};

}
}
