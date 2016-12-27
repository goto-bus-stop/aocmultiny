#pragma once
#include <dplobby.h>
#include <stdlib.h>
#include <string>

namespace dplib {

/**
 * A DPName represents a player or session name. DirectPlay names have both
 * "short" and "long" variants, but dplib's DPName currently only supports its
 * "short" variant.
 */
class DPName {
private:
  std::string name;
  DPNAME* dpName;

public:
  /**
   * Create a new DPName.
   *
   * @param name The player or session name.
   */
  DPName (std::string name);
  ~DPName ();

  /**
   * Allocate the raw DirectPlay DPNAME structure.
   * `unwrap` calls this automatically if necessary, so this method should
   * normally not be used directly.
   */
  void alloc ();
  /**
   * Get the raw DirectPlay DPNAME described by this instance.
   */
  DPNAME* unwrap ();

  /**
   * Parse a raw DirectPlay DPNAME structure into a DPName instance.
   */
  static DPName* parse (DPNAME* raw);
};

}
