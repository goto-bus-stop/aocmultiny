#pragma once
#include "DPAddress.hpp"
#include "DPGame.hpp"
#include "DPSession.hpp"
#include "dputil.hpp"
#include <dplay.h>
#include <dplobby.h>

using std::string;

namespace dplib {

class DPSession;

class DPLobbyMessage {
private:
  bool shouldStop;
public:
  int appId;
  DPSession* session;
  int flags;
  int size;
  void* data;

  /**
   * Create a new Lobby Message.
   *
   * @param appId
   * @param flags
   * @param data
   * @param size
   */
  DPLobbyMessage (int appId, int flags, void* data, int size);
  ~DPLobbyMessage ();

  /**
   * Send a reply to this message.
   *
   * @param data Pointer to a buffer containing reply data.
   * @param size Size of the reply data buffer.
   */
  void reply (void* data, int size);

  /**
   * Stop listening for new lobby messages in the future.
   */
  void stop ();

  /**
   * Check if the incoming lobby messages loop is being asked to exit.
   */
  bool requestedStop ();

  template<typename T>
  T as ();
};

class DPLobby {
private:
  IDirectPlay4A* dp;
  IDirectPlayLobby3A* dpLobby = NULL;

  /**
   * Create the internal IDirectPlayLobby instance.
   */
  HRESULT create ();

  DPLobby ();

public:
  /**
   * Get a global DPLobby instance.
   */
  static DPLobby* get ();

  /**
   * Retrieve the raw DirectPlay lobby instance.
   */
  IDirectPlayLobby3A* getInternalLobby ();

  /**
   * Get DirectPlay connection settings for a session. If used in a lobby
   * client, the appId of the relevant application has to be passed in.
   * Otherwise, the settings for the current application are returned.
   */
  DPLConnection* getConnectionSettings (DWORD appId = 0);
  HRESULT setConnectionSettings (DWORD appId, DPLConnection* connection);

  /**
   * Create a new session for a game.
   *
   * @param game The game to play.
   */
  DPSession* createSession (DPGame* game);
  DPSession* hostSession (DPGame* game);
  DPSession* hostSession (DPGame* game, DPAddress* address);
  DPSession* joinSession (DPGame* game, DPAddress* address, GUID sessionGuid);

  DPLobbyMessage* receiveLobbyMessage (DWORD appId);
  void sendLobbyMessage (int flags, int appId, void* data, int size);
};

}
