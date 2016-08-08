#pragma once
#include <string>
#include <dplobby.h>

namespace dplib {

class DPSessionDesc {
private:
  GUID guidApplication;
  GUID guidInstance;
  std::string name;
  std::string pass;
  bool host;
  DPSESSIONDESC2* dpSessionDesc;

public:
  DPSessionDesc (GUID guidApplication, GUID guidInstance, std::string sessionName, std::string sessionPass, bool host);
  ~DPSessionDesc ();
  bool isHost ();

  void alloc ();
  DPSESSIONDESC2* unwrap ();

  static DPSessionDesc* parse (DPSESSIONDESC2* raw, bool isHost);
};

}
