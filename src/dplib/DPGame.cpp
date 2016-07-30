#include "DPGame.hpp"
#include "DPLobby.hpp"
#include <iostream>

using namespace std;

namespace aocmultiny {
namespace dplib {

DPGame::DPGame (GUID guid)
    :
    guid(guid) {
}

GUID DPGame::getGameGuid () {
  return this->guid;
}

void DPGame::receiveMessage (DPLobbyMessage* message) {
  if (message->flags == DPLMSG_STANDARD) {
    wcout << "[DPGame::receiveMessage] received STANDARD message, discarding" << endl;
    return;
  }
  auto lobby = message->lobby;
  auto sysMsg = static_cast<DPLMSG_SYSTEMMESSAGE*>(message->data);
  wcout << "[DPGame::receiveMessage] received SYSTEMMESSAGE, using default processing" << endl;
  switch (sysMsg->dwType) {
  case DPLSYS_APPTERMINATED:
    wcout << "[DPGame::receiveMessage] received APPTERMINATED message" << endl;
    lobby->onAppTerminated.emit();
    message->stop();
    return;
  case DPLSYS_GETPROPERTY: {
    auto getPropMsg = static_cast<DPLMSG_GETPROPERTY*>(message->data);
    auto getPropRes = new DPLMSG_GETPROPERTYRESPONSE;
    getPropRes->dwType = DPLSYS_GETPROPERTYRESPONSE;
    getPropRes->dwRequestID = getPropMsg->dwRequestID;
    getPropRes->guidPlayer = getPropMsg->guidPlayer;
    getPropRes->guidPropertyTag = getPropMsg->guidPropertyTag;
    getPropRes->hr = DPERR_UNKNOWNMESSAGE;
    getPropRes->dwDataSize = 0;
    getPropRes->dwPropertyData[0] = 0;
    wcout << "[DPGame::receiveMessage] responding to unknown GETPROPERTY message" << endl;
    message->reply(getPropRes, sizeof(getPropRes));
    break;
  }
  case DPLSYS_NEWSESSIONHOST:
    wcout << "[DPGame::receiveMessage] received NEWSESSIONHOST message" << endl;
    break;
  case DPLSYS_CONNECTIONSETTINGSREAD:
    wcout << "[DPGame::receiveMessage] received CONNECTIONSETTINGSREAD message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTFAILED:
    wcout << "[DPGame::receiveMessage] received CONNECTFAILED message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTSUCCEEDED:
    wcout << "[DPGame::receiveMessage] received CONNECTSUCCEEDED message!" << endl;
    lobby->onConnectSucceeded.emit();
    break;
  default:
    wcout << "[DPGame::receiveMessage] received unknown message: " << sysMsg->dwType << endl;
    break;
  }
}

}
}
