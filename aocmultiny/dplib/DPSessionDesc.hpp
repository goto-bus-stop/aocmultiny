#pragma once
#include <string>
#include <dplobby.h>

namespace aocmultiny {
namespace dplib {

class DPSessionDesc {

private:
  GUID guidInstance;
  std::wstring name;
  std::wstring pass;
  bool host;
  DPSESSIONDESC2* dpSessionDesc;

public:
  DPSessionDesc (GUID guidInstance, std::wstring sessionName, std::wstring sessionPass, bool host);
  ~DPSessionDesc ();
  bool isHost ();

  void alloc ();
  DPSESSIONDESC2* unwrap ();

};

}
}
