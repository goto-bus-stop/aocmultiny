#include "DPApplication.hpp"
#include "DPLobby.hpp"
#include <dplobby.h>
#include <string>

using std::string;
using std::wstring;

namespace dplib {

/**
 * TODO Actually allow unicode stuff. Until then DPApplication forces ANSI for
 * everything, like the rest of dplib.
 *
 * TODO Implement launcher property. The dplobby.h file in use here doesn't
 * include DPAPPLICATIONDESC2. Should update that & then implement launcher()
 * methods.
 */

static char* copyString (string source) {
  if (source.empty()) {
    return NULL;
  }

  const auto length = source.size();
  auto dest = new char[length + 1];
  dest[length] = '\0';
  memcpy(dest, source.c_str(), length);
  return dest;
}

static wchar_t* copyString (wstring source) {
  if (source.empty()) {
    return NULL;
  }

  const auto length = source.size();
  auto dest = new wchar_t[length + 1];
  dest[length] = '\0';
  memcpy(dest, source.c_str(), length);
  return dest;
}

static string wtoa (wstring source) {
  string dest = "";
  for (auto c : source) {
    dest.push_back(static_cast<char>(c));
  }
  return dest;
}

DPApplication::DPApplication (GUID guid) {
  this->dpDesc = new DPAPPLICATIONDESC;
  memset(this->dpDesc, 0, sizeof(DPAPPLICATIONDESC));

  this->dpDesc->dwSize = sizeof(DPAPPLICATIONDESC);
  this->dpDesc->dwFlags = 0;
  this->dpDesc->guidApplication = guid;
}

DPApplication::~DPApplication () {
  if (this->dpDesc->lpszApplicationNameA) {
    free(this->dpDesc->lpszApplicationNameA);
  }
  if (this->dpDesc->lpszFilenameA) {
    free(this->dpDesc->lpszFilenameA);
  }
  if (this->dpDesc->lpszPathA) {
    free(this->dpDesc->lpszPathA);
  }
  if (this->dpDesc->lpszCommandLineA) {
    free(this->dpDesc->lpszCommandLineA);
  }
  if (this->dpDesc->lpszCurrentDirectoryA) {
    free(this->dpDesc->lpszCurrentDirectoryA);
  }
  if (this->dpDesc->lpszDescriptionA) {
    free(this->dpDesc->lpszDescriptionA);
  }
  if (this->dpDesc->lpszDescriptionW) {
    free(this->dpDesc->lpszDescriptionW);
  }
  delete this->dpDesc;
}

DPApplication* DPApplication::appName (string name) {
  this->dpDesc->lpszApplicationNameA = copyString(name);
  return this;
}

DPApplication* DPApplication::appName (wstring name) {
  return this->appName(wtoa(name));
}

DPApplication* DPApplication::filename (string name) {
  this->dpDesc->lpszFilenameA = copyString(name);
  return this;
}

DPApplication* DPApplication::filename (wstring name) {
  return this->filename(wtoa(name));
}

DPApplication* DPApplication::path (string name) {
  this->dpDesc->lpszPathA = copyString(name);
  return this;
}

DPApplication* DPApplication::path (wstring name) {
  return this->path(wtoa(name));
}

DPApplication* DPApplication::commandLine (string name) {
  this->dpDesc->lpszCommandLineA = copyString(name);
  return this;
}

DPApplication* DPApplication::commandLine (wstring name) {
  return this->commandLine(wtoa(name));
}

DPApplication* DPApplication::description (string name) {
  this->dpDesc->lpszDescriptionA = copyString(name);
  return this;
}

DPApplication* DPApplication::description (wstring name) {
  this->dpDesc->lpszDescriptionW = copyString(name);
  return this;
}

DPApplication* DPApplication::currentDirectory (string name) {
  this->dpDesc->lpszCurrentDirectoryA = copyString(name);
  return this;
}

DPApplication* DPApplication::currentDirectory (wstring name) {
  return this->currentDirectory(wtoa(name));
}

DPApplication* DPApplication::launcher (string name) {
  // No-op.

  // this->dpDesc->lpszAppLauncherNameA = copyString(name);
  return this;
}

DPApplication* DPApplication::launcher (wstring name) {
  // No-op.

  // this->dpDesc->lpszAppLauncherName = copyString(name);
  return this;
}

DPAPPLICATIONDESC* DPApplication::unwrap () {
  /* No-op.
  this->dpDesc->dwSize = this->dpDesc->lpszAppLauncherName == NULL
    ? sizeof(DPAPPLICATIONDESC)
    : sizeof(DPAPPLICATIONDESC2);
  */

  return this->dpDesc;
}

HRESULT DPApplication::register_ () {
  return DPLobby::get()->registerApplication(this);
}

DPApplication* DPApplication::create (GUID guid) {
  return new DPApplication(guid);
}

}
