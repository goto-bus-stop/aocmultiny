#pragma once
#include <dplobby.h>

#define DEFINE_DPGAME(name, guid) \
  class name: public aocmultiny::dplib::DPGame { \
  public: \
    name () : DPGame(guid) {}; \
    GUID getGameGuid (); \
    void receiveMessage (aocmultiny::dplib::DPLobbyMessage* message); \
  }

namespace dplib {

class DPLobbyMessage;

class DPGame {
private:
  GUID guid;
public:
  /**
   * Create a new DPGame.
   *
   * @param gameGuid GUID of the application.
   */
  DPGame (GUID gameGuid);

  /**
   * Get the application GUID.
   */
  GUID getGameGuid ();

  /**
   * Handler for DirectPlay lobby messages. Subclasses can implement
   * application-specific messages here.
   *
   * @param message Incoming lobby message.
   */
  virtual void receiveMessage (DPLobbyMessage* message);
};

}
