#include "DPAddress.hpp"
#include <dplobby.h>
#include <string>

namespace aocmultiny {
namespace dplib {

DPAddress::DPAddress (LPDIRECTPLAYLOBBY3A lobby, std::string ip)
    :
    lobby(lobby),
    ip(ip) {
  this->alloc();
}

void DPAddress::alloc () {
  auto addressElements =
    (DPCOMPOUNDADDRESSELEMENT*) malloc(3 * sizeof(DPCOMPOUNDADDRESSELEMENT));
  int elements = 0;
  void* address;
  DWORD addressSize = 0;

  // TCP/IP service provider
  addressElements[0].guidDataType = DPAID_ServiceProvider;
  addressElements[0].lpData = (void*)&DPSPGUID_TCPIP;
  addressElements[0].dwDataSize = sizeof(GUID);
  elements++;
  // Host IP
  addressElements[1].guidDataType = DPAID_INet;
  addressElements[1].lpData = (void*)this->ip.c_str();
  addressElements[1].dwDataSize = this->ip.length() + 1;
  elements++;

  auto hr = this->lobby->CreateCompoundAddress(addressElements, 2, NULL, &addressSize);
  if (hr == DPERR_BUFFERTOOSMALL) {
    this->size = addressSize;
    address = malloc(addressSize);
    if (address == NULL) return;
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
}
