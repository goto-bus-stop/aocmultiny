#pragma once
#include <dplobby.h>

#define DEFINE_DPGAME(name, guid) \
  class name: public DPGame { \
  public: \
    name : DPGame () {}; \
    GUID getGameGuid () { \
      return guid; \
    }; \
  }

namespace aocmultiny {
namespace dplib {

class DPGame {
public:
  DPGame ();
  virtual GUID getGameGuid ();
};

}
}
