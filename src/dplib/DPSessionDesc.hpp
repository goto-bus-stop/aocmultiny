#pragma once
#include <string>
#include <dplobby.h>

namespace aocmultiny {
namespace dplib {

class DPSessionDesc {

private:
  GUID guidInstance;
  std::string name;
  std::string pass;
  bool host;
  DPSESSIONDESC2* dpSessionDesc;

public:
  DPSessionDesc (GUID guidInstance, std::string sessionName, std::string sessionPass, bool host);
  ~DPSessionDesc ();
  bool isHost ();

  void alloc ();
  DPSESSIONDESC2* unwrap ();

};

}
}
