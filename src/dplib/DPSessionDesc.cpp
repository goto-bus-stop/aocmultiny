#include <stdlib.h>
#include <iostream>
#include "DPSessionDesc.hpp"

using namespace std;

namespace dplib {

wstring GUIDToString (GUID guid) {
  wchar_t* str = static_cast<wchar_t*>(malloc(51 * sizeof(wchar_t)));
  StringFromGUID2(guid, str, 50);
  return str;
}

DPSessionDesc::DPSessionDesc (GUID guidApplication, GUID guidInstance, string sessionName, string sessionPass, bool host)
    :
    guidApplication(guidApplication),
    guidInstance(guidInstance),
    name(sessionName),
    pass(sessionPass),
    host(host) {
  this->alloc();
}

bool DPSessionDesc::isHost () {
  return this->host;
}

void DPSessionDesc::alloc () {
  auto sessionDesc = new DPSESSIONDESC2;
  sessionDesc->dwSize = sizeof(DPSESSIONDESC2);
  sessionDesc->dwFlags = 0;
  wcout << "[DPSessionDesc::alloc] guidInstance: " << GUIDToString(guidInstance) << endl;
  sessionDesc->guidInstance = guidInstance;
  sessionDesc->guidApplication = guidApplication;
  sessionDesc->dwMaxPlayers = 8;
  sessionDesc->dwCurrentPlayers = 0;
  if (host) {
    sessionDesc->lpszSessionNameA = const_cast<char*>(this->name.c_str());
  } else {
    sessionDesc->lpszSessionName = NULL;
  }
  sessionDesc->lpszPassword = NULL;
  // All reserved
  sessionDesc->dwReserved1 = 0;
  sessionDesc->dwReserved2 = 0;
  sessionDesc->dwUser1 = 0;
  sessionDesc->dwUser2 = 0;
  sessionDesc->dwUser3 = 0;
  sessionDesc->dwUser4 = 0;
  this->dpSessionDesc = sessionDesc;
}

DPSESSIONDESC2* DPSessionDesc::unwrap () {
  return this->dpSessionDesc;
}

DPSessionDesc::~DPSessionDesc () {
  free(this->dpSessionDesc->lpszSessionNameA);
  delete this->dpSessionDesc;
}

}
