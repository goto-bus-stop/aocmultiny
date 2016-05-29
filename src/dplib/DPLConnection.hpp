#pragma once
#include <dplobby.h>
#include "DPAddress.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"

namespace aocmultiny {
namespace dplib {

class DPLConnection {

private:
  DPAddress address;
  DPSessionDesc* sessionDesc;
  DPName* name;
  DPLCONNECTION* dpConnection;

public:
  DPLConnection (DPAddress address, DPSessionDesc* sessionDesc, DPName* playerName);
  ~DPLConnection ();
  void alloc ();
  DPLCONNECTION* unwrap ();

};

}
}
