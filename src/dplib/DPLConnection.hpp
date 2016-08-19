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
  /**
   * Create a new DirectPlay Lobby Connection.
   *
   * @param address
   * @param sessionDesc
   * @param playerName
   */
  DPLConnection (DPAddress* address, DPSessionDesc* sessionDesc, DPName* playerName);
  ~DPLConnection ();

  /**
   * Allocate the raw DirectPlay DPLCONNECTION.
   * `unwrap` calls this automatically if necessary, so this method should
   * normally not be used directly.
   */
  void alloc ();
  /**
   * Get the raw DirectPlay DPLCONNECTION as described by this instance.
   */
  DPLCONNECTION* unwrap ();

  /**
   * Parse a raw DirectPlay DPLCONNECTION into a DPLConnection instance.
   */
  static DPLConnection* parse (DPLCONNECTION* raw);
};

}
