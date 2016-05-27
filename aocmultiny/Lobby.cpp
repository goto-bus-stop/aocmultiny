#include <string>
#include <iostream>
#include "Lobby.hpp"
#include "dplib/main.hpp"

using namespace std;

namespace aocmultiny {

Lobby::Lobby () {
  CoCreateInstance(
    CLSID_DirectPlay,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectPlay4A,
    (void**)&this->dp
  );

  this->create();
}

HRESULT Lobby::create () {
  HRESULT hr = CoCreateInstance(
    CLSID_DirectPlayLobby,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IDirectPlayLobby3A,
    (void**)&this->dpLobby
  );
  return hr;
}

/**
 * Host a game.
 */
void Lobby::host () {
  dplib::DPAddress address (this->dpLobby, "");
  this->launch(address, true);
  return;
}

/**
 * Join a game.
 */
void Lobby::join (string remoteIp) {
  dplib::DPAddress address (this->dpLobby, remoteIp);
  return;
}

GUID createGuid () {
  GUID guid;
  CoCreateGuid(&guid);
  return guid;
}

bool Lobby::receiveMessage (DWORD appId) {
  DWORD messageFlags;
  DWORD dataSize = 0;
  DWORD responseSize = 0;
  HRESULT hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, NULL, &dataSize);
  if (hr != DPERR_BUFFERTOOSMALL) {
    wcout << "[Lobby::receiveMessage] Error: should be BUFFERTOOSMALL" << endl;
    return false;
  }
  messageFlags = 0;
  void* data = malloc(dataSize);
  hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, data, &dataSize);
  wcout << "[Lobby::receiveMessage] received " << messageFlags << " : " << dataSize << endl;
  if (FAILED(hr)) {
    return false;
  }
  if (messageFlags == DPLMSG_STANDARD) {
    wcout << "[Lobby::receiveMessage] received STANDARD message, discarding" << endl;
    free(data);
    return true;
  }
  DPLMSG_SYSTEMMESSAGE* sysMsg = (DPLMSG_SYSTEMMESSAGE*) data;
  DPLMSG_GETPROPERTY* getPropMsg;
  DPLMSG_GETPROPERTYRESPONSE* getPropRes;
  wcout << "[Lobby::receiveMessage] received SYSTEMMESSAGE, processing" << endl;
  switch (sysMsg->dwType) {
  case DPLSYS_APPTERMINATED:
    wcout << "[Lobby::receiveMessage] received APPTERMINATED message" << endl
              << "Press <enter> to exit." << endl;
    return false;
    break;
  case DPLSYS_GETPROPERTY:
    wcout << "[Lobby::receiveMessage] received GETPROPERTY message" << endl;
    getPropMsg = (DPLMSG_GETPROPERTY*) data;
    responseSize = sizeof(DPLMSG_GETPROPERTYRESPONSE);
    getPropRes = (DPLMSG_GETPROPERTYRESPONSE*) malloc(responseSize);
    getPropRes->dwType = DPLSYS_GETPROPERTYRESPONSE;
    getPropRes->dwRequestID = getPropMsg->dwRequestID;
    getPropRes->guidPlayer = getPropMsg->guidPlayer;
    getPropRes->guidPropertyTag = getPropMsg->guidPropertyTag;
    getPropRes->hr = DPERR_UNKNOWNMESSAGE;
    getPropRes->dwDataSize = 0;
    getPropRes->dwPropertyData[0] = 0;
    wcout << "[Lobby::receiveMessage] responding to unknown GETPROPERTY message" << endl;
    this->dpLobby->SendLobbyMessage(0, appId, getPropRes, responseSize);
    break;
  case DPLSYS_NEWSESSIONHOST:
    wcout << "[Lobby::receiveMessage] received NEWSESSIONHOST message" << endl;
    break;
  case DPLSYS_CONNECTIONSETTINGSREAD:
    wcout << "[Lobby::receiveMessage] received CONNECTIONSETTINGSREAD message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTFAILED:
    wcout << "[Lobby::receiveMessage] received CONNECTFAILED message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTSUCCEEDED:
    wcout << "[Lobby::receiveMessage] received CONNECTSUCCEEDED message!" << endl;
    break;
  default:
    wcout << "[Lobby::receiveMessage] received unknown message: " << (int) sysMsg->dwType << endl;
    break;
  }

  free(data);
  return true;
}

void Lobby::launch (dplib::DPAddress address, bool isHosting) {
  GUID guid = createGuid();
  dplib::DPSessionDesc* sessionDesc = new dplib::DPSessionDesc(guid, L"Session", L"", isHosting);
  dplib::DPName* playerName = new dplib::DPName(L"My Name");
  dplib::DPLConnection* connection = new dplib::DPLConnection(address, sessionDesc, playerName);

  HANDLE receiveEvent = CreateEvent(NULL, false, false, NULL);
  DWORD appId = 0;

  wcout << "[Lobby::launch] Launching app" << endl;

  HRESULT hr = this->dpLobby->RunApplication(0, &appId, connection->unwrap(), receiveEvent);
  if (FAILED(hr)) {
    // oops.
    wcout << "[Lobby::launch] Something went wrong:" << endl;
    cout << dplib::getDPError(hr) << endl;
  } else {
    //HANDLE thread = CreateThread(NULL, 0, receiveThread, &receiveEvent, 0, NULL);
    bool keepGoing = true;
    while (keepGoing && WaitForSingleObject(receiveEvent, INFINITE) == WAIT_OBJECT_0) {
      wcout << "[Lobby::launch] receiving lobby message" << endl;
      keepGoing = this->receiveMessage(appId);
    }
  }

  delete connection;
  delete playerName;
  delete sessionDesc;
}

}
