#pragma once
#include <dplobby.h>
#include <stdlib.h>
#include <string>

namespace aocmultiny {
namespace dplib {

class DPName {

private:
  std::wstring name;
  DPNAME* dpName;

public:
  DPName (std::wstring name);
  ~DPName ();
  void alloc ();
  DPNAME* unwrap ();

};

}
}
