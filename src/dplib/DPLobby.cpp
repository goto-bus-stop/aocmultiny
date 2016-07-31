#include <string>
#include <iostream>
#include "DPLobby.hpp"
#include "DPGame.hpp"

using namespace std;

namespace dplib {

DPLobbyMessage::DPLobbyMessage (DPLobby* lobby, int appId, int flags, void* data, int size)
    :
    appId(appId),
    lobby(lobby),
    flags(flags),
    size(size),
    data(data) {
}

void DPLobbyMessage::reply (void* data, int size) {
  this->lobby->sendLobbyMessage(0, this->appId, data, size);
}

void DPLobbyMessage::stop () {
  this->shouldStop = true;
}

bool DPLobbyMessage::requestedStop () {
  return this->shouldStop;
}

template<typename T>
T DPLobbyMessage::as () {
  return static_cast<T>(this->data);
}

DPLobbyMessage::~DPLobbyMessage () {
  free(this->data);
}

DPLobby::DPLobby (DPGame* game)
    :
    DPLobby(game, "Player") {
}

DPLobby::DPLobby (DPGame* game, string playerName)
    :
    game(game),
    guid({ 0 }),
    isHosting(false),
    playerName(playerName) {
  CoCreateInstance(
    CLSID_DirectPlay,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectPlay4A,
    reinterpret_cast<void**>(&this->dp)
  );

  this->create();
}

HRESULT DPLobby::create () {
  auto hr = CoCreateInstance(
    CLSID_DirectPlayLobby,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectPlayLobby3A,
    reinterpret_cast<void**>(&this->dpLobby)
  );
  return hr;
}

/**
 * Host a game.
 */
void DPLobby::host () {
  CoCreateGuid(&this->guid);
  this->isHosting = true;
  this->hostIp = "";
  return;
}

/**
 * Join a game.
 */
void DPLobby::join (GUID sessionId, string remoteIp) {
  this->guid = sessionId;
  this->hostIp = remoteIp;
  return;
}

GUID DPLobby::getSessionGUID () {
  return this->guid;
}

bool DPLobby::receiveMessage (DWORD appId) {
  DWORD messageFlags;
  DWORD dataSize = 0;
  auto hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, NULL, &dataSize);
  if (hr != DPERR_BUFFERTOOSMALL) {
    wcout << "[DPLobby::receiveMessage] Error: should be BUFFERTOOSMALL" << endl;
    return false;
  }
  messageFlags = 0;
  void* data = malloc(dataSize);
  hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, data, &dataSize);
  wcout << "[DPLobby::receiveMessage] received " << messageFlags << " : " << dataSize << endl;
  if (FAILED(hr)) {
    return false;
  }

  auto message = new DPLobbyMessage(this, appId, messageFlags, data, dataSize);

  this->game->receiveMessage(message);

  auto stopping = message->requestedStop();
  delete message;
  return !stopping;
}

void DPLobby::sendLobbyMessage (int flags, int appId, void* data, int size) {
  this->dpLobby->SendLobbyMessage(flags, appId, data, size);
}

void DPLobby::launch () {
  DPAddress address (this->dpLobby, this->hostIp);
  const auto gameGuid = this->game->getGameGuid();
  const auto sessionDesc = new DPSessionDesc(gameGuid, this->guid, "Session", "", this->isHosting);
  const auto playerName = new DPName(this->playerName);
  const auto connection = new DPLConnection(address, sessionDesc, playerName);

  auto receiveEvent = CreateEvent(NULL, false, false, NULL);
  DWORD appId = 0;

  wcout << "[DPLobby::launch] Launching app" << endl;

  auto hr = this->dpLobby->RunApplication(0, &appId, connection->unwrap(), receiveEvent);
  if (FAILED(hr)) {
    // oops.
    wcout << "[DPLobby::launch] Something went wrong:" << endl;
    cout << getDPError(hr) << endl;
  } else {
    bool keepGoing = true;
    while (keepGoing && WaitForSingleObject(receiveEvent, INFINITE) == WAIT_OBJECT_0) {
      wcout << "[DPLobby::launch] receiving lobby message" << endl;
      keepGoing = this->receiveMessage(appId);
    }
  }

  wcout << "[DPLobby::launch] Cleaning up" << endl;
  delete connection;
  delete playerName;
  delete sessionDesc;
}

}
