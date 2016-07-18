#include <iostream>
#include <dplobby.h>
#include "DPLobby.hpp"
#include "DPGameAoC.hpp"

using namespace std;

namespace aocmultiny {
namespace dplib {

void DPGameAoC::receiveMessage (DPLobbyMessage* message) {
  if (message->flags == DPLMSG_STANDARD) {
    wcout << "[DPGameAoC::receiveMessage] received STANDARD message, discarding" << endl;
    return;
  }
  auto lobby = message->lobby;
  auto sysMsg = static_cast<DPLMSG_SYSTEMMESSAGE*>(message->data);
  wcout << "[DPGameAoC::receiveMessage] received SYSTEMMESSAGE, processing" << endl;
  switch (sysMsg->dwType) {
  case DPLSYS_APPTERMINATED:
    wcout << "[DPGameAoC::receiveMessage] received APPTERMINATED message" << endl
          << "Press <enter> to exit." << endl;
    lobby->onAppTerminated.emit();
    message->stop();
    return;
  case DPLSYS_GETPROPERTY: {
    wcout << "[DPGameAoC::receiveMessage] received GETPROPERTY message" << endl;
    auto getPropMsg = static_cast<DPLMSG_GETPROPERTY*>(message->data);
    auto responseSize = sizeof(DPLMSG_GETPROPERTYRESPONSE);
    auto getPropRes = static_cast<DPLMSG_GETPROPERTYRESPONSE*>(malloc(responseSize));
    getPropRes->dwType = DPLSYS_GETPROPERTYRESPONSE;
    getPropRes->dwRequestID = getPropMsg->dwRequestID;
    getPropRes->guidPlayer = getPropMsg->guidPlayer;
    getPropRes->guidPropertyTag = getPropMsg->guidPropertyTag;
    getPropRes->hr = DPERR_UNKNOWNMESSAGE;
    getPropRes->dwDataSize = 0;
    getPropRes->dwPropertyData[0] = 0;
    wcout << "[DPGameAoC::receiveMessage] responding to unknown GETPROPERTY message" << endl;
    message->reply(getPropRes, responseSize);
    break;
  }
  case DPLSYS_NEWSESSIONHOST:
    wcout << "[DPGameAoC::receiveMessage] received NEWSESSIONHOST message" << endl;
    break;
  case DPLSYS_CONNECTIONSETTINGSREAD:
    wcout << "[DPGameAoC::receiveMessage] received CONNECTIONSETTINGSREAD message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTFAILED:
    wcout << "[DPGameAoC::receiveMessage] received CONNECTFAILED message" << endl;
    break;
  case DPLSYS_DPLAYCONNECTSUCCEEDED:
    wcout << "[DPGameAoC::receiveMessage] received CONNECTSUCCEEDED message!" << endl;
    lobby->onConnectSucceeded.emit();
    break;
  default:
    wcout << "[DPGameAoC::receiveMessage] received unknown message: " << sysMsg->dwType << endl;
    break;
  }
}

}
}