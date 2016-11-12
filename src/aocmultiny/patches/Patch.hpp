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

  virtual void install ();
};

}
}
