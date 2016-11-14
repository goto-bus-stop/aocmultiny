#pragma once
#include "Patch.hpp"
#include <dplib/DPApplication.hpp>
#include <string>

using std::wstring;

namespace aocmultiny {
namespace patches {

const GUID GUID_UserPatch = { 0xe85d0af7, 0x8f5a, 0x4dc4, { 0xa3, 0xda, 0x17, 0x3c, 0x82, 0x93, 0xd0, 0xba } };

struct UPConfig {
  UPConfig ();
  wstring str ();
  // Features
  bool core;
  bool savegameFormat;
  bool widescreen;
  bool windowedMode;
  bool upnp;
  // Registry settings
  bool multipleQueue;
  bool originalPatrolDelay;
  bool extendPopulationCaps;
  bool replaceSnowWithGrass;
  bool alternateRed;
  bool alternatePurple;
  bool alternateGrey;
  // Restricted features
  bool leftAlignedInterface;
  bool delinkVolume;
  bool precisionScrolling;
  bool lowFps;
  bool extendedHotkeys;
  bool forceGameplayFeatures;
  bool displayOreResource;
  bool multiplayerAntiCheat;
};

class UserPatch: public Patch {
private:
  UPConfig config;
  wstring installerPath;
  wstring filename;
  wstring directory;
  wstring path;

  void downloadInstaller ();
  void runInstaller ();
  void removeInstaller ();
public:
  UserPatch ();
  virtual void install ();
};

}
}
