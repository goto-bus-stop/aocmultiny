#include "Lobby.hpp"

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
    (void**)this->dpLobby
  );
  return hr;
}

void Lobby::host () {
  return;
}

}
