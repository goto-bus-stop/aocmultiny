#include <stdlib.h>
#include <iostream>
#include "DPSessionDesc.hpp"
#include "../util.hpp"

using namespace std;

namespace dplib {

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

GUID DPSessionDesc::getApplicationGuid () {
  return this->guidApplication;
}

GUID DPSessionDesc::getSessionGuid () {
  return this->guidInstance;
}

string DPSessionDesc::getName () {
  return this->name;
}

string DPSessionDesc::getPassword () {
  return this->pass;
}

void DPSessionDesc::alloc () {
  auto sessionDesc = new DPSESSIONDESC2;
  sessionDesc->dwSize = sizeof(DPSESSIONDESC2);
  sessionDesc->dwFlags = 0;
  wcout << "[DPSessionDesc::alloc] guidInstance: " << to_wstring(guidInstance) << endl;
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

DPSessionDesc* DPSessionDesc::parse (DPSESSIONDESC2* raw, bool isHost) {
  return new DPSessionDesc(
    raw->guidApplication,
    raw->guidInstance,
    raw->lpszSessionNameA != NULL ? raw->lpszSessionNameA : "",
    raw->lpszPasswordA != NULL ? raw->lpszPasswordA : "",
    isHost
  );
}

}
