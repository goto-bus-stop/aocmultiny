#include <stdlib.h>
#include <string>
#include <dplobby.h>
#include "DPName.hpp"

namespace aocmultiny {
namespace dplib {

DPName::DPName (std::wstring name)
    :
    name(name) {
  this->alloc();
}

void DPName::alloc () {
  this->dpName = (DPNAME*) malloc(sizeof(DPNAME));

  wchar_t* nameStr = (wchar_t*) calloc(this->name.length() + 1, sizeof(wchar_t));
  this->name.copy(nameStr, this->name.length(), 0);
  this->dpName->dwSize = sizeof(DPNAME);
  this->dpName->dwFlags = 0;
  this->dpName->lpszShortName = nameStr;
  this->dpName->lpszLongName = nameStr;
}

DPNAME* DPName::unwrap () {
  return this->dpName;
}

DPName::~DPName () {
  free(this->dpName->lpszLongName);
  free(this->dpName);
}

}
}
