#include <string>
#include "Lobby.hpp"
#include "dplib/main.hpp"

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
  return;
}

/**
 * Join a game.
 */
void Lobby::join (std::string remoteIp) {
  dplib::DPAddress address (this->dpLobby, remoteIp);
  return;
}

}
