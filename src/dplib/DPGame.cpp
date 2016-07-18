#include "DPGame.hpp"

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
}

}
}
