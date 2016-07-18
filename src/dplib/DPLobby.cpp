#include <string>
#include <iostream>
#include "DPLobby.hpp"

using namespace std;

namespace aocmultiny {
namespace dplib {

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
  if (messageFlags == DPLMSG_STANDARD) {
    wcout << "[DPLobby::receiveMessage] received STANDARD message, discarding" << endl;
    free(data);
    return true;
  }
  auto sysMsg = static_cast<DPLMSG_SYSTEMMESSAGE*>(data);
  wcout << "[DPLobby::receiveMessage] received SYSTEMMESSAGE, processing" << endl;
  switch (sysMsg->dwType) {
  case DPLSYS_APPTERMINATED:
    wcout << "[DPLobby::receiveMessage] received APPTERMINATED message" << endl
          << "Press <enter> to exit." << endl;
    this->onAppTerminated.emit();
    return false;
  case DPLSYS_GETPROPERTY: {
    wcout << "[DPLobby::receiveMessage] received GETPROPERTY message" << endl;
    auto getPropMsg = static_cast<DPLMSG_GETPROPERTY*>(data);
    auto responseSize = sizeof(DPLMSG_GETPROPERTYRESPONSE);
    auto getPropRes = static_cast<DPLMSG_GETPROPERTYRESPONSE*>(malloc(responseSize));
    getPropRes->dwType = DPLSYS_GETPROPERTYRESPONSE;
    getPropRes->dwRequestID = getPropMsg->dwRequestID;
    getPropRes->guidPlayer = getPropMsg->guidPlayer;
    getPropRes->guidPropertyTag = getPropMsg->guidPropertyTag;
    getPropRes->hr = DPERR_UNKNOWNMESSAGE;
    getPropRes->dwDataSize = 0;
    getPropRes->dwPropertyData[0] = 0;
    wcout << "[DPLobby::receiveMessage] responding to unknown GETPROPERTY message" << endl;
    this->dpLobby->SendLobbyMessage(0, appId, getPropRes, responseSize);
    break;
  }
  case DPLSYS_NEWSESSIONHOST:
    wcout << "[DPLobby::receiveMessage] received NEWSESSIONHOST message" << endl;
    break;
  case DPLSYS_CONNECTIONSETTINGSREAD:
    wcout << "[DPLobby::receiveMessage] received CONNECTIONSETTINGSREAD message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTFAILED:
    wcout << "[DPLobby::receiveMessage] received CONNECTFAILED message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTSUCCEEDED:
    wcout << "[DPLobby::receiveMessage] received CONNECTSUCCEEDED message!" << endl;
    this->onConnectSucceeded.emit();
    break;
  default:
    wcout << "[DPLobby::receiveMessage] received unknown message: " << sysMsg->dwType << endl;
    break;
  }

  free(data);
  return true;
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
}
