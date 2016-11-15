#include "Patch.hpp"
#include "../util/Registry.hpp"
#include "../util/VDFFile.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <wx/progdlg.h>

namespace aocmultiny {
namespace patches {

using std::find_if;
using std::isdigit;
using std::string;
using std::vector;
using std::wstring;
using aocmultiny::util::Registry;
using aocmultiny::util::VDFFile;

Patch::Patch () {
  this->progressDialog = new wxProgressDialog(
    "Patch installation",
    "Waiting",
    100,
    NULL,
    wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT |
    wxPD_SMOOTH | wxPD_REMAINING_TIME
  );
}
Patch::~Patch () {
  delete this->progressDialog;
}

void Patch::install () {}

void Patch::step (wstring message) {
  std::wcout << "[Patch::step] Indeterminate: " << message << std::endl;
  this->progressDialog->Pulse(message);
  this->progressDialog->Show();
}

void Patch::step (wstring message, int max) {
  std::wcout << "[Patch::step] Determinate: " << message << std::endl;
  this->progressDialog->SetRange(max);
  this->progressDialog->Update(0, message);
  this->progressDialog->Show();
}

void Patch::progress () {
  this->progressDialog->Pulse();
}

void Patch::progress (int current) {
  if (current != this->progressDialog->GetRange()) {
    this->progressDialog->Update(current);
  }
}

wstring Patch::getAoCDirectory () {
  Registry registry (HKEY_LOCAL_MACHINE);
  auto apps = registry["Software"]["Microsoft"]["DirectPlay"]["Applications"];
  auto key = apps["Age of Empires II - The Conquerors Expansion"];
  return key.readString("CurrentDirectory");
}

wstring Patch::getHDDirectory () {
  auto libraryFolders = vector<string>();
  string steamPath;

  try {
    Registry registry (HKEY_CURRENT_USER);
    auto steamPathW = registry["SOFTWARE"]["Valve"]["Steam"].readString("SteamPath");
    for (auto c : steamPathW) steamPath.push_back(static_cast<char>(c));
  } catch (std::out_of_range) {
    std::cerr << "No Steam installation found." << std::endl;
    return L"";
  }

  std::cout << "Steam install path: " << steamPath << std::endl;

  try {
    std::cout << "Vdf: " << steamPath + "\\SteamApps\\libraryfolders.vdf" << std::endl;

    auto library = VDFFile(steamPath + "\\SteamApps\\libraryfolders.vdf").parse();
    libraryFolders.push_back(steamPath + "\\SteamApps");
    // Numeric LibraryFolders keys signify additional library folders.
    for (const auto& entry : library["LibraryFolders"]) {
      auto key = entry.first;
      auto isNumeric = find_if(key.begin(), key.end(), [] (auto c) { return !isdigit(c); }) == key.end();
      if (isNumeric && entry.second.isValue()) {
        libraryFolders.push_back(string(entry.second) + "\\SteamApps");
      }
    }
  } catch (const std::exception &e) {
    // Can't catch ios_base::failure here because of
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145
    std::cerr << "Could not find HD Edition install path: " << e.what() << std::endl;
    return L"";
  }

  std::cout << "Detected library folders: " << std::endl;
  for (const auto& folder : libraryFolders) {
    std::cout << " - " << folder << std::endl;
  }

  // HD Edition manifest file.
  auto manifest = "appmanifest_221380.acf";
  for (const auto& folder : libraryFolders) {
    std::cout << "Attempting manifest: " << folder + "\\" + manifest << std::endl;
    try {
      auto app = VDFFile(folder + "\\" + manifest).parse();
      auto dir = folder + "\\" + string(app["AppState"]["installdir"]);
      std::cout << "Detected directory: " << dir << std::endl;
      wstring wide;
      for (const auto c : dir) wide.push_back(c);
      return wide;
    } catch (const std::exception &e) {
      // Ignore. Probably just doesn't exist.
    }
  }
  return L"";
}

wstring Patch::getTempDirectory () {
  auto path = new wchar_t[261];
  if (GetTempPath(261, path)) {
    // Ensure the directory actually exists.
    CreateDirectory(path, NULL);
    return path;
  }
  return L".";
}

}
}
