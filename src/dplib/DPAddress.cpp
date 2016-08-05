#include "DPAddress.hpp"
#include "DPLobby.hpp"
#include "../util.hpp"
#include <dplobby.h>
#include <string>
#include <iostream>
#include <tuple>

using std::pair;
using std::to_wstring;

namespace dplib {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

DPAddress::DPAddress ()
    :
    elements(vector<DPCOMPOUNDADDRESSELEMENT*>()),
    address(NULL),
    addressSize(0) {
}

DPAddress::DPAddress (GUID serviceProvider)
    :
    DPAddress() {
  this->add(DPAID_ServiceProvider, &serviceProvider, sizeof(serviceProvider));
}

DPAddress* DPAddress::add (DPCOMPOUNDADDRESSELEMENT* element) {
  this->elements.push_back(element);

  return this;
}

DPAddress* DPAddress::add (GUID type, void* data, DWORD dataSize) {
  auto element = new DPCOMPOUNDADDRESSELEMENT;
  element->guidDataType = type;
  element->lpData = data;
  element->dwDataSize = dataSize;

  return this->add(element);
}

size_t DPAddress::size () {
  return this->elements.size();
}

void DPAddress::alloc () {
  const auto count = this->size();
  const auto addressElements = new DPCOMPOUNDADDRESSELEMENT[count];

  for (size_t i = 0; i < count; i++) {
    addressElements[i] = *(this->elements[i]);
  }

  void* address;
  DWORD addressSize = 0;

  auto lobby = DPLobby::get()->getInternalLobby();
  auto hr = lobby->CreateCompoundAddress(addressElements, count, NULL, &addressSize);
  if (hr == DPERR_BUFFERTOOSMALL) {
    address = new BYTE[addressSize];
    hr = lobby->CreateCompoundAddress(addressElements, count, address, &addressSize);
    if (FAILED(hr)) {
      return;
    }
    this->address = address;
    this->addressSize = addressSize;
  }
}

pair<void*, DWORD> DPAddress::unwrap() {
  if (!this->address) {
    this->alloc();
  }
  std::wcout << "[DPAddress::unwrap] size = " << this->addressSize << std::endl;
  return { this->address, this->addressSize };
}

DPAddress::~DPAddress () {
  if (this->address) {
    free(this->address);
  }
}

DPAddress* DPAddress::ip (string ip) {
  auto address = new DPAddress(DPSPGUID_TCPIP);

  auto size = ip.size();
  void* ipStr = new char[size];
  memcpy(ipStr, ip.c_str(), size);
  address->add(DPAID_INet, ipStr, size);

  return address;
}

}
