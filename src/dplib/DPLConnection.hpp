#pragma once
#include <dplobby.h>
#include "DPAddress.hpp"
#include "DPName.hpp"
#include "DPSessionDesc.hpp"

namespace dplib {

class DPLConnection {
private:
  DPAddress* address;
  DPSessionDesc* sessionDesc;
  DPName* name;
  DPLCONNECTION* dpConnection;

public:
  DPLConnection (DPAddress* address, DPSessionDesc* sessionDesc, DPName* playerName);
  ~DPLConnection ();
  // FIXME remove this parameter when DPLobby is a singleton
  void alloc (IDirectPlayLobby3A* lobby);
  DPLCONNECTION* unwrap ();
};

}
