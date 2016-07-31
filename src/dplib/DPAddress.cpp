#include "DPAddress.hpp"
#include "../util.hpp"
#include <dplobby.h>
#include <string>
#include <iostream>

using std::to_wstring;

namespace dplib {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

DPAddress::DPAddress (LPDIRECTPLAYLOBBY3A lobby, std::string ip)
    :
    lobby(lobby),
    ip(ip) {
  this->alloc();
}

void DPAddress::alloc () {
  auto addressElements = new DPCOMPOUNDADDRESSELEMENT[2];
  int elements = 0;
  void* address;
  DWORD addressSize = 0;

  // TCP/IP service provider
  addressElements[0].guidDataType = DPAID_ServiceProvider;
  addressElements[0].lpData = const_cast<void*>(
    static_cast<const void*>(&DPSPGUID_TCPIP));
  addressElements[0].dwDataSize = sizeof(GUID);
  elements++;
  // Host IP
  addressElements[1].guidDataType = DPAID_INet;
  addressElements[1].lpData = const_cast<void*>(
    static_cast<const void*>(this->ip.c_str()));
  addressElements[1].dwDataSize = this->ip.length() + 1;
  elements++;

  std::wcout << "[DPAddress::alloc] SPID: " << to_wstring(DPSPGUID_NICE) << std::endl;

  auto hr = this->lobby->CreateCompoundAddress(addressElements, 2, NULL, &addressSize);
  if (hr == DPERR_BUFFERTOOSMALL) {
    this->size = addressSize;
    address = new BYTE[addressSize];
    hr = this->lobby->CreateCompoundAddress(addressElements, elements, address, &addressSize);
    if (FAILED(hr)) {
      free(address);
      address = NULL;
      return;
    }
    this->address = address;
  }
}

void* DPAddress::unwrap () {
  return this->address;
}

}
