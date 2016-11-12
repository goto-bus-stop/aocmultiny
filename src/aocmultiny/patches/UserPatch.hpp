#pragma once
#include "Patch.hpp"
#include <string>

using std::wstring;

namespace aocmultiny {
namespace patches {

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
public:
  virtual void install ();
};

}
}
