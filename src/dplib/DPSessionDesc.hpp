#pragma once
#include <string>
#include <dplobby.h>

using std::string;

namespace dplib {

class DPSessionDesc {
private:
  GUID guidApplication;
  GUID guidInstance;
  string name;
  string pass;
  bool host;
  DPSESSIONDESC2* dpSessionDesc;

public:
  DPSessionDesc (GUID guidApplication, GUID guidInstance, string sessionName, string sessionPass, bool host);
  ~DPSessionDesc ();
  bool isHost ();
  GUID getApplicationGuid ();
  GUID getSessionGuid ();
  string getName ();
  string getPassword ();

  void alloc ();
  DPSESSIONDESC2* unwrap ();

  static DPSessionDesc* parse (DPSESSIONDESC2* raw, bool isHost);
};

}
