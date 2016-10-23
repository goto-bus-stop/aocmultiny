#include "NiceServiceProvider.hpp"
#include "nicesp.hpp"
#include "../util.hpp"
#include <dplib/DPLobby.hpp>
#include <dplib/dputil.hpp>
#include <glib.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace nicesp {

GMainLoop* mainLoop;

static HRESULT init (SPINITDATA* spData) {
  g_debug("SPInit");

  g_info(
    "[SPInit] Initializing library for %s (%ls)",
    to_string(*spData->lpGuid).c_str(),
    spData->lpszName
  );

  mainLoop = g_main_loop_new(NULL, false);

  if (IsEqualGUID(*spData->lpGuid, DPSPGUID_NICE)) {
    auto sp = new NiceServiceProvider(spData);
    return sp->init();
  }

  return DPERR_UNAVAILABLE;
}

}

__declspec(dllexport) HRESULT __cdecl SPInit (SPINITDATA* spData) {
  return nicesp::init(spData);
}

G_LOCK_DEFINE(loglock);
static void logToFile (const gchar* domain, GLogLevelFlags, const gchar* message, void* ctx) {
  auto output = static_cast<ofstream*>(ctx);
  G_LOCK(loglock);
  if (domain != NULL) {
    *output << domain << "  " << message << endl;
  }
  if (message != NULL) {
    *output << "glog " << message << endl;
  }
  G_UNLOCK(loglock);
}

streambuf* coutbuf;
ofstream logfile;
static void setupLogging () {
  logfile.open("C:\\nicesp-" + to_string(time(0)) + ".txt");
  // Send g_log messages to file
  g_log_set_default_handler(logToFile, &logfile);
  // Redirect cout to log file
  coutbuf = cout.rdbuf();
  cout.rdbuf(logfile.rdbuf());
}
static void teardownLogging () {
  cout.rdbuf(coutbuf);
  logfile.close();
}

BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, void* reserved) {
  g_debug("DllMain (0x%p, %ld, %p)", instance, reason, reserved);

  switch (reason) {
  case DLL_PROCESS_ATTACH:
    setupLogging();

    g_message("--------------------");
    g_message("[DllMain] INITIALISE");
    g_message("--------------------");

    DisableThreadLibraryCalls(instance);
    break;
  case DLL_PROCESS_DETACH:
    g_message("-----------------");
    g_message("[DllMain] RELEASE");
    g_message("-----------------");

    teardownLogging();
    break;
  default:
    break;
  }
  return TRUE;
}
