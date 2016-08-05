#include "DPAddress.hpp"
#include "../util.hpp"
#include <dplobby.h>
#include <string>
#include <iostream>

using std::to_wstring;

namespace dplib {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

DPAddress::DPAddress ()
    :
    elements(vector<DPCOMPOUNDADDRESSELEMENT*>()) {
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

DPAddress* DPAddress::add (GUID type, void* data, int dataSize) {
  auto element = new DPCOMPOUNDADDRESSELEMENT;
  element->guidDataType = type;
  element->lpData = data;
  element->dwDataSize = dataSize;

  return this->add(element);
}

size_t DPAddress::size () {
  return this->elements.size();
}

DPCOMPOUNDADDRESSELEMENT* DPAddress::alloc () {
  const auto elements = this->size();
  const auto addressElements = new DPCOMPOUNDADDRESSELEMENT[elements];

  for (size_t i = 0; i < elements; i++) {
    addressElements[i] = *(this->elements[i]);
  }

  return addressElements;
}

DPAddress* DPAddress::ip (string ip) {
  auto address = new DPAddress(DPSPGUID_TCPIP);

  auto size = ip.size();
  void* ipStr = new char[size];
  memcpy(ipStr, ip.c_str(), size);

  return address->add(DPAID_INet, ipStr, size);
}

}
