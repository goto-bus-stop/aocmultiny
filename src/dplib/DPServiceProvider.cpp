#include "DPServiceProvider.hpp"
#include <iostream>
#include <vector>
#include "debug.hpp"

using std::vector;

namespace dplib {

// Hacky stuff, please ignore. (Or see DPServiceProvider::getFrom.)
static vector<DPServiceProvider*> providers;

struct DPSPData {
  unsigned int provider;
};

DPServiceProvider* DPServiceProvider::getFrom (IDirectPlaySP* sp) {
  // We just pretend it takes the DPServiceProvider from SPData, but really it
  // takes indexes into a global providers vector from SPData. That's because
  // the pointers inside the instance got messed up when I tried putting it or a
  // pointer to it straight into SPData. This works, so ¯\_(ツ)_/¯
  DPSPData* data;
  DWORD size;
  auto hr = sp->GetSPData(reinterpret_cast<void**>(&data), &size, DPGET_LOCAL);
  if (SUCCEEDED(hr)) {
    std::cout << "[DPServiceProvider::getFrom] " << data->provider << std::endl;
    return providers.at(data->provider);
  }
  return nullptr;
}

DPServiceProvider::DPServiceProvider (SPINITDATA* spData)
    :
    spData(spData),
    callbacks(spData->lpCB),
    spInstance(spData->lpISP),
    version(DPSP_MAJORVERSIONMASK & DPSP_MAJORVERSION) {
  // Pretend to store instance in the raw DirectPlay service provider's SPData.
  auto providerId = providers.size();
  providers.push_back(this);
  DPSPData data = { providerId };
  this->getInternalServiceProvider()->SetSPData(&data, sizeof(data), DPSET_LOCAL);
}

HRESULT DPServiceProvider::handleMessage (void* data, int size, void* header) {
  return this->getInternalServiceProvider()
    ->HandleMessage(data, size, header);
}

// Default implementations of callbacks. Most return "DPERR_UNSUPPORTED".

// The default GetCaps sets some probably-close-enough-usually capabilities, and
// no flags by default. TODO: perhaps add an overload so you can return a
// capability struct instead of setting properties?
HRESULT DPServiceProvider::GetCaps (DPSP_GETCAPSDATA* data) {
  // Some defaults...
  data->lpCaps->dwFlags = 0;
  data->lpCaps->dwMaxBufferSize = 1024;
  data->lpCaps->dwMaxQueueSize = 0;
  data->lpCaps->dwMaxPlayers = 65536;
  data->lpCaps->dwHundredBaud = 0;
  data->lpCaps->dwLatency = 50;
  data->lpCaps->dwMaxLocalPlayers = 65536;
  data->lpCaps->dwHeaderLength = 0;
  data->lpCaps->dwTimeout = 500;

  return DP_OK;
}

// Simplified EnumSessions.
HRESULT DPServiceProvider::EnumSessions (void* message, int size, bool returnStatus) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::EnumSessions (DPSP_ENUMSESSIONSDATA* data) {
  return this->EnumSessions(
    data->lpMessage,
    data->dwMessageSize,
    data->bReturnStatus
  );
}

// Simplified Reply.
HRESULT DPServiceProvider::Reply (void* spHeader, void* message, int size, DPID nameServer) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::Reply (DPSP_REPLYDATA* data) {
  return this->Reply(
    data->lpSPMessageHeader,
    data->lpMessage,
    data->dwMessageSize,
    data->idNameServer
  );
}
HRESULT DPServiceProvider::Send (DPSP_SENDDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::DeletePlayer (DPSP_DELETEPLAYERDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::GetAddress (DPSP_GETADDRESSDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::Open (DPSP_OPENDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::Close () {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::Shutdown () {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::GetAddressChoices (DPSP_GETADDRESSCHOICESDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::SendEx (DPSP_SENDEXDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::SendToGroupEx (DPSP_SENDTOGROUPEXDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::Cancel (DPSP_CANCELDATA* data) {
  return DPERR_UNSUPPORTED;
}
HRESULT DPServiceProvider::GetMessageQueue (DPSP_GETMESSAGEQUEUEDATA* data) {
  return DPERR_UNSUPPORTED;
}

// Callbacks for the DirectPlay service provider code. All these functions do is
// to call the right method on the correct DPServiceProvider instance. They're
// necessary because we can't pass references to C++ methods to DirectPlay
// directly.

static HRESULT WINAPI triageEnumSessions(DPSP_ENUMSESSIONSDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->EnumSessions(data);
}
static HRESULT WINAPI triageReply(DPSP_REPLYDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Reply(data);
}
static HRESULT WINAPI triageSend(DPSP_SENDDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Send(data);
}
static HRESULT WINAPI triageCreatePlayer(DPSP_CREATEPLAYERDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->CreatePlayer(data);
}
static HRESULT WINAPI triageDeletePlayer(DPSP_DELETEPLAYERDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->DeletePlayer(data);
}
static HRESULT WINAPI triageGetAddress(DPSP_GETADDRESSDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->GetAddress(data);
}
static HRESULT WINAPI triageGetCaps(DPSP_GETCAPSDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->GetCaps(data);
}
static HRESULT WINAPI triageOpen(DPSP_OPENDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Open(data);
}
static HRESULT WINAPI triageCloseEx(DPSP_CLOSEDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Close();
}
static HRESULT WINAPI triageShutdownEx(DPSP_SHUTDOWNDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Shutdown();
}
static HRESULT WINAPI triageGetAddressChoices(DPSP_GETADDRESSCHOICESDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->GetAddressChoices(data);
}
static HRESULT WINAPI triageSendEx(DPSP_SENDEXDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->SendEx(data);
}
static HRESULT WINAPI triageSendToGroupEx(DPSP_SENDTOGROUPEXDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->SendToGroupEx(data);
}
static HRESULT WINAPI triageCancel(DPSP_CANCELDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->Cancel(data);
}
static HRESULT WINAPI triageGetMessageQueue(DPSP_GETMESSAGEQUEUEDATA* data) {
  std::cout << data << std::endl;
  return DPServiceProvider::getFrom(data->lpISP)->GetMessageQueue(data);
}

HRESULT DPServiceProvider::init () {
  auto callbacks = this->callbacks;
  callbacks->EnumSessions = triageEnumSessions;
  callbacks->Reply = triageReply;
  callbacks->Send = triageSend;
  callbacks->CreatePlayer = triageCreatePlayer;
  callbacks->DeletePlayer = triageDeletePlayer;
  callbacks->GetAddress = triageGetAddress;
  callbacks->GetCaps = triageGetCaps;
  callbacks->Open = triageOpen;
  callbacks->CloseEx = triageCloseEx;
  callbacks->ShutdownEx = triageShutdownEx;
  callbacks->GetAddressChoices = triageGetAddressChoices;
  callbacks->SendEx = triageSendEx;
  callbacks->SendToGroupEx = triageSendToGroupEx;
  callbacks->Cancel = triageCancel;
  callbacks->GetMessageQueue = triageGetMessageQueue;

  // TODO make header size configurable?
  this->spData->dwSPHeaderSize = 0;
  this->spData->dwSPVersion = this->version;

  return DP_OK;
}

}