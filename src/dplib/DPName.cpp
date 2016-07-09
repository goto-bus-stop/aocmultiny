#include <stdlib.h>
#include <string>
#include <dplobby.h>
#include "DPName.hpp"

namespace aocmultiny {
namespace dplib {

DPName::DPName (std::string name)
    :
    name(name) {
  this->alloc();
}

void DPName::alloc () {
  this->dpName = static_cast<DPNAME*>(malloc(sizeof(DPNAME)));

  auto length = this->name.length();
  auto nameStr = static_cast<char*>(calloc(length + 1, sizeof(char)));
  this->name.copy(nameStr, length, 0);

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
  free(this->dpName);
}

}
}
