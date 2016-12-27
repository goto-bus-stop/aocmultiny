#include <stdlib.h>
#include <string>
#include <dplobby.h>
#include "DPName.hpp"

namespace dplib {

DPName::DPName (std::string name)
    :
    name(name) {
  this->alloc();
}

void DPName::alloc () {
  this->dpName = new DPNAME;

  const auto length = this->name.size();
  auto nameStr = new char[length + 1];
  nameStr[length] = '\0';
  memcpy(nameStr, this->name.c_str(), length);

  this->dpName->dwSize = sizeof(DPNAME);
  this->dpName->dwFlags = 0;
  this->dpName->lpszShortNameA = nameStr;
  this->dpName->lpszLongNameA = nameStr;
}

DPNAME* DPName::unwrap () {
  return this->dpName;
}

DPName::~DPName () {
  free(this->dpName->lpszLongNameA);
  delete this->dpName;
}

DPName* DPName::parse (DPNAME* raw) {
  return new DPName(raw->lpszLongNameA);
}

}
