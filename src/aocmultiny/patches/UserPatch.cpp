#include "UserPatch.hpp"
#include <dplib/DPApplication.hpp>
#include <wx/utils.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <string>
#include <iostream>
#include "../util/CurlInputStream.hpp"

using std::string;
using dplib::DPApplication;

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

UserPatch::UserPatch () {
}

void UserPatch::initialize () {
  this->filename = L"age2_aocmultiny.exe";
  this->directory = this->getAoCDirectory() + L"\\age2_x1";
  this->path = this->directory + L"\\" + this->filename;
  this->installerPath = this->getAoCDirectory() + L"\\aocmultiny-SetupAoC.exe";
}

// TODO should probably host it somewhere of our own instead. With HTTPS and
// *just* the installer so we don't need the zip stuff.
void UserPatch::downloadInstaller () {
  // wxHTTP get;
  // get.SetTimeout(10);
  // while (!get.Connect("userpatch.aiscripters.net")) {
  //   wxSleep(1);
  // }
  // auto httpStream = get.GetInputStream("/UserPatch.v1.4.20150723-000000.zip");
  auto httpStream = new aocmultiny::util::CurlInputStream();
  httpStream->get("http://userpatch.aiscripters.net/UserPatch.v1.4.20150723-000000.zip");

  auto zipStream = new wxZipInputStream(httpStream);

  auto outputStream = new wxFileOutputStream(this->installerPath);

  wxZipEntry* entry;
  float progress = 0.0f;
  while ((entry = zipStream->GetNextEntry())) {
    std::cout << "[UserPatch::downloadInstaller] " << entry->GetName() << std::endl;
    if (entry->GetName() == "UserPatch\\SetupAoC.exe") {
      zipStream->OpenEntry(*entry);
      // outputStream->Write(*zipStream);

      auto buffer = new char[4096];
      while (!zipStream->Eof()) {
        if (!zipStream->Read(buffer, 4096)) {
          std::cerr << "Could not read: " << zipStream->GetLastError() << std::endl;
        } else {
          auto chunkSize = zipStream->LastRead();
          auto totalSize = zipStream->GetSize();
          progress += chunkSize;
          int progressValue = (progress / (float) totalSize) * 100.0f;
          outputStream->WriteAll(buffer, chunkSize);
          this->progress(progressValue);
        }
      }
      delete buffer;

      zipStream->CloseEntry();
      break;
    }
  }

  delete outputStream;
  delete zipStream;
  // get.Close();
}

void UserPatch::runInstaller () {
  wxExecute(this->installerPath + L" -i -f:" + this->config.str(), wxEXEC_SYNC);

  auto app = DPApplication::create(GUID_UserPatch)
    ->appName("Age of Empires II: UserPatch v1.4 RC")
    ->filename(this->filename)
    ->path(this->directory)
    ->currentDirectory(this->getAoCDirectory())
    ->commandLine("lobby");

  app->register_();

  delete app;
}

void UserPatch::removeInstaller () {
  _wremove(this->installerPath.c_str());
}

void UserPatch::install () {
  this->step(L"Initializing");
  this->initialize();

  this->step(L"Downloading");
  this->downloadInstaller();

  this->step(L"Installing");
  this->runInstaller();

  this->step(L"Cleaning up");
  this->removeInstaller();
}

}
}
