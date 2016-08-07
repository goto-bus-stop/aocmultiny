#pragma once
#include <dplobby.h>
#include <stdlib.h>
#include <string>

namespace dplib {

class DPName {

private:
  std::string name;
  DPNAME* dpName;

public:
  DPName (std::string name);
  ~DPName ();
  void alloc ();
  DPNAME* unwrap ();

};

}
