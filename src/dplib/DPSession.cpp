#include <string>
#include <iostream>
#include "DPSession.hpp"
#include "DPGame.hpp"

using namespace std;

namespace dplib {

DPSession::DPSession (DPGame* game)
    :
    lobby(DPLobby::get()),
    game(game),
    guid({ 0 }),
    isHosting(false) {
}

DPSession::DPSession (DPGame* game, string playerName)
    :
    DPSession(game) {
  this->setPlayerName(playerName);
}

DPSession* DPSession::setPlayerName (string playerName) {
  this->playerName = playerName;
  return this;
}

/**
 * Host a game.
 */
DPSession* DPSession::host (DPAddress* address) {
  CoCreateGuid(&this->guid);
  this->isHosting = true;
  this->address = address;
  return this;
}

/**
 * Join a game.
 */
DPSession* DPSession::join (DPAddress* address, GUID sessionId) {
  this->address = address;
  this->guid = sessionId;
  return this;
}

GUID DPSession::getSessionGUID () {
  return this->guid;
}

bool DPSession::receiveMessage (DWORD appId) {
  auto message = this->lobby->receiveLobbyMessage(appId);
  if (!message) {
    return false;
  }

  message->session = this;

  this->game->receiveMessage(message);

  auto stopping = message->requestedStop();
  delete message;
  return !stopping;
}

void DPSession::sendLobbyMessage (int flags, int appId, void* data, int size) {
  this->lobby->sendLobbyMessage(flags, appId, data, size);
}

void DPSession::launch () {
  const auto gameGuid = this->game->getGameGuid();
  const auto sessionDesc = new DPSessionDesc(gameGuid, this->guid, "Session", "", this->isHosting);
  const auto playerName = new DPName(this->playerName);
  const auto connection = new DPLConnection(this->address, sessionDesc, playerName);

  auto receiveEvent = CreateEvent(NULL, false, false, NULL);
  DWORD appId = 0;

  wcout << "[DPSession::launch] Launching app" << endl;

  auto hr = this->lobby->getInternalLobby()
    ->RunApplication(0, &appId, connection->unwrap(), receiveEvent);
  if (FAILED(hr)) {
    // oops.
    wcout << "[DPSession::launch] Something went wrong:" << endl;
    cout << getDPError(hr) << endl;
  } else {
    wcout << "[DPSession::launch] appId: " << appId << endl;
    bool keepGoing = true;
    while (keepGoing && WaitForSingleObject(receiveEvent, INFINITE) == WAIT_OBJECT_0) {
      wcout << "[DPSession::launch] receiving lobby message" << endl;
      keepGoing = this->receiveMessage(appId);
    }
  }

  wcout << "[DPSession::launch] Cleaning up" << endl;
}

}
