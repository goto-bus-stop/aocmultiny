#pragma once
#include "Patch.hpp"
#include "../util/Registry.hpp"
#include <string>

using std::wstring;
using aocmultiny::util::Registry;

namespace aocmultiny {
namespace patches {

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

}
}
