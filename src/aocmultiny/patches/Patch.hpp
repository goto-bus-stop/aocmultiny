#pragma once
#include <string>

using std::wstring;

namespace aocmultiny {
namespace patches {

class Patch {
public:
  Patch ();
  ~Patch ();

  wstring getAoCDirectory ();
  wstring getHDDirectory ();
  wstring getTempDirectory ();

  virtual void install ();
};

}
}
