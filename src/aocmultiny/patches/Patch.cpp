#include "Patch.hpp"
#include "../util/Registry.hpp"
#include "../util/VDFFile.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

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
}
Patch::~Patch () {
}

void Patch::install () {}

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

  try {
    auto library = VDFFile(steamPath + "\\SteamApps\\libraryfolders.vdf").parse();
    libraryFolders.push_back(steamPath + "\\SteamApps");
    // Numeric LibraryFolders keys signify additional library folders.
    for (auto const& entry : library["LibraryFolders"]) {
      auto key = entry.first;
      auto isNumeric = find_if(key.begin(), key.end(), [] (auto c) { return !isdigit(c); }) == key.end();
      if (isNumeric && entry.second.isValue()) {
        libraryFolders.push_back(string(entry.second));
      }
    }

    // HD Edition manifest file.
    auto manifest = "appmanifest_221380.acf";
    for (auto const& folder : libraryFolders) {
      auto app = VDFFile(folder + "\\" + manifest).parse();
      auto dir = folder + string(app["AppState"]["installdir"]);
      wstring wide;
      for (auto const c : dir) wide.push_back(c);
      return wide;
    }
  } catch (const std::exception &e) {
    // Can't catch ios_base::failure here because of
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145
    std::cerr << "Could not find HD Edition install path: " << e.what() << std::endl;
    return L"";
  }
  return L"";
}

wstring Patch::getTempDirectory () {
  auto path = new wchar_t[261];
  if (GetTempPath(261, path)) {
    return path;
  }
  return L".";
}

}
}
