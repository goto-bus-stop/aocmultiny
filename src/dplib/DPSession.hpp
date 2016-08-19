#pragma once
#include "DPAddress.hpp"
#include "DPLConnection.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"
#include "DPLobby.hpp"
#include "util.hpp"
#include "../util.hpp"
#include <dplay.h>
#include <dplobby.h>
#include <string>

using std::string;
using aocmultiny::EventListeners;

namespace dplib {

class DPLobby;

/**
 * A DPSession represents a Game session.
 */
class DPSession {
private:
  DPLobby* lobby;
  DPGame* game;

  GUID guid;
  bool isHosting;
  DPAddress* address;

  string playerName;

  bool receiveMessage (DWORD appId);

public:
  DPSession (DPGame* game, string playerName);
  DPSession (DPGame* game);

  /**
   * Set the name of the local player.
   */
  DPSession* setPlayerName (string playerName);

  /**
   * Configure this session as being hosted "here"--the local player is the
   * host.
   */
  DPSession* host (DPAddress* address);
  /**
   * Join a session.
   *
   * @param address Session address, used by DirectPlay to find the host.
   * @param sessionId GUID of the session to join.
   */
  DPSession* join (DPAddress* address, GUID sessionId);

  /**
   * Get the GUID of this session. This method does not return anything useful
   * until either the host() or join() methods have been called.
   */
  GUID getSessionGUID ();

  /**
   * Send a lobby message to the application.
   *
   * @param flags Message flags, see the DirectX Documentation.
   * @param appId ID of the recipient application.
   * @param data Pointer to a buffer containing message data.
   * @param size Size of the message data buffer.
   */
  void sendLobbyMessage (int flags, int appId, void* data, int size);

  /**
   * Launch the application. This method does not return until the application
   * exits, so it should be called in a separate thread.
   */
  void launch ();
  /**
   * Event: Fired when the application has connected to the session.
   */
  EventListeners<> onConnectSucceeded;
  /**
   * Event: Fired when the application has quit.
   */
  EventListeners<> onAppTerminated;
};

}
