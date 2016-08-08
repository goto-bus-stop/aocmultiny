#include <string>
#include <iostream>
#include "DPLobby.hpp"
#include "DPGame.hpp"
#include "DPSession.hpp"

using namespace std;

namespace dplib {

DPLobbyMessage::DPLobbyMessage (int appId, int flags, void* data, int size)
    :
    appId(appId),
    flags(flags),
    size(size),
    data(data) {
}

void DPLobbyMessage::reply (void* data, int size) {
  auto lobby = DPLobby::get();
  lobby->sendLobbyMessage(0, this->appId, data, size);
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

DPLobby* DPLobby::get () {
  static DPLobby* instance;
  if (!instance) {
    instance = new DPLobby();
  }
  return instance;
}

DPLobby::DPLobby () {
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

IDirectPlayLobby3A* DPLobby::getInternalLobby () {
  return this->dpLobby;
}

DPLConnection* DPLobby::getConnectionSettings (DWORD appId) {
  DWORD size;
  this->dpLobby->GetConnectionSettings(appId, NULL, &size);
  auto data = new BYTE[size];
  auto hr = this->dpLobby->GetConnectionSettings(appId, data, &size);
  if (FAILED(hr)) {
    return NULL;
  }
  auto dpConnection = reinterpret_cast<DPLCONNECTION*>(data);
  return DPLConnection::parse(dpConnection);
}

HRESULT DPLobby::setConnectionSettings (DWORD appId, DPLConnection* connection) {
  return this->dpLobby->SetConnectionSettings(0, appId, connection->unwrap());
}

DPLobbyMessage* DPLobby::receiveLobbyMessage (DWORD appId) {
  DWORD messageFlags;
  DWORD dataSize = 0;
  auto hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, NULL, &dataSize);
  if (hr != DPERR_BUFFERTOOSMALL) {
    wcout << "[DPLobby::receiveLobbyMessage] Error: should be BUFFERTOOSMALL" << endl;
    return NULL;
  }
  messageFlags = 0;
  void* data = malloc(dataSize);
  hr = this->dpLobby->ReceiveLobbyMessage(0, appId, &messageFlags, data, &dataSize);
  wcout << "[DPLobby::receiveLobbyMessage] received " << messageFlags << " : " << dataSize << endl;
  if (FAILED(hr)) {
    return NULL;
  }

  return new DPLobbyMessage(appId, messageFlags, data, dataSize);
}

void DPLobby::sendLobbyMessage (int flags, int appId, void* data, int size) {
  this->dpLobby->SendLobbyMessage(flags, appId, data, size);
}

DPSession* DPLobby::createSession (DPGame* game) {
  return new DPSession(game);
}

/**
 * Host a session using the default TCP/IP service provider.
 */
DPSession* DPLobby::hostSession (DPGame* game) {
  return this->hostSession(game, DPAddress::ip(""));
}

/**
 * Host a session using a custom address.
 */
DPSession* DPLobby::hostSession (DPGame* game, DPAddress* address) {
  auto session = this->createSession(game);
  session->host(address);
  return session;
}

/**
 * Join a session on a remote machine.
 */
DPSession* DPLobby::joinSession (DPGame* game, DPAddress* address, GUID sessionGuid) {
  auto session = this->createSession(game);
  session->join(address, sessionGuid);
  return session;
}

}
