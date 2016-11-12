#include "UserPatch.hpp"
#include <wx/utils.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <string>
#include <iostream>

using std::string;

namespace aocmultiny {
namespace patches {

UPConfig::UPConfig ()
    :
    core(true),
    savegameFormat(true),
    widescreen(true),
    windowedMode(true),
    upnp(true),
    multipleQueue(true),
    originalPatrolDelay(false),
    extendPopulationCaps(true),
    replaceSnowWithGrass(false),
    alternateRed(false),
    alternatePurple(false),
    alternateGrey(false),
    leftAlignedInterface(false),
    delinkVolume(false),
    precisionScrolling(false),
    lowFps(false),
    extendedHotkeys(true),
    forceGameplayFeatures(false),
    displayOreResource(false),
    multiplayerAntiCheat(true) {
}

wchar_t getOption (bool opt) {
  return opt ? '1' : '0';
}
wstring UPConfig::str () {
  return wstring({
    getOption(this->core),
    getOption(this->savegameFormat),
    getOption(this->widescreen),
    getOption(this->windowedMode),
    getOption(this->upnp),

    getOption(this->multipleQueue),
    getOption(this->originalPatrolDelay),
    getOption(this->extendPopulationCaps),
    getOption(this->replaceSnowWithGrass),
    getOption(this->alternateRed),
    getOption(this->alternatePurple),
    getOption(this->alternateGrey),

    getOption(this->leftAlignedInterface),
    getOption(this->delinkVolume),
    getOption(this->precisionScrolling),
    getOption(this->lowFps),
    getOption(!this->extendedHotkeys),
    getOption(this->forceGameplayFeatures),
    getOption(this->displayOreResource),
    getOption(!this->multiplayerAntiCheat),
  });
}

// TODO should probably host it somewhere of our own instead. With HTTPS and
// *just* the installer so we don't need the zip stuff.
static void downloadInstaller (wstring exeFile) {
  wxHTTP get;
  get.SetTimeout(10);
  while (!get.Connect("userpatch.aiscripters.net")) {
    wxSleep(1);
  }
  auto httpStream = get.GetInputStream("/UserPatch.v1.4.20150723-000000.zip");
  auto zipStream = new wxZipInputStream(httpStream);

  auto outputStream = new wxFileOutputStream(exeFile);

  wxZipEntry* entry;
  while ((entry = zipStream->GetNextEntry())) {
    if (entry->GetName() == "UserPatch\\SetupAoC.exe") {
      zipStream->OpenEntry(*entry);
      outputStream->Write(*zipStream);
      zipStream->CloseEntry();
      break;
    }
  }

  delete outputStream;
  delete zipStream;
}

static void runInstaller (wstring path, UPConfig config) {
  wxExecute(path + L" -i -f:" + config.str(), wxEXEC_SYNC);
}

void UserPatch::install () {
  auto exeFile = L"aocmultiny-SetupAoC.exe";
  auto dir = this->getAoCDirectory();
  auto path = dir + L"\\" + exeFile;
  UPConfig config;

  downloadInstaller(path);
  runInstaller(path, config);
}

}
}
