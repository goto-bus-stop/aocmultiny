#pragma once
#include <dplobby.h>
#include <string>
#include <vector>
#include <tuple>

using std::pair;
using std::string;
using std::vector;

namespace dplib {

/**
 * A DirectPlay address.
 */
class DPAddress {
private:
  vector<DPCOMPOUNDADDRESSELEMENT*> elements;
  void* address;
  DWORD addressSize = 0;

public:
  /**
   * Create a new, empty DPAddress.
   */
  DPAddress ();
  /**
   * Create a new DPAddress containing a service provider ID.
   * @param serviceProvider The Service Provider GUID.
   */
  DPAddress (GUID serviceProvider);
  ~DPAddress ();

  /**
   * Add a raw DirectPlay compound address element to the address.
   * @param element
   */
  DPAddress* add (DPCOMPOUNDADDRESSELEMENT* element);
  /**
   * Add an element to the address.
   *
   * @param type Address element type GUID.
   * @param data Pointer to a buffer containing the element data.
   * @param dataSize Size of the element data buffer.
   */
  DPAddress* add (GUID type, const void* data, DWORD dataSize);
  /**
   * Add an element to the address.
   *
   * @param type Address element type GUID.
   * @param value Element data. In this case, the `sizeof` operator is used to
   *              determine the element data size.
   */
  template<typename Value>
  DPAddress* add (GUID type, Value value);

  /**
   * Retrieve an element from the address by type.
   *
   * @param type Desired address element type GUID.
   */
  pair<void*, DWORD> get (GUID type);

  /**
   * Get the amount of elements in the address.
   */
  size_t size ();
  /**
   * Allocate the address using DirectPlay's CreateCompoundAddress method.
   * `unwrap` calls this automatically if necessary, so this method should
   * normally not be used directly.
   */
  void alloc ();
  /**
   * Get the raw DirectPlay compound address described by this DPAddress
   * instance. This can be passed to other DirectPlay methods.
   */
  pair<void*, DWORD> unwrap ();

  /**
   * Parse a raw DirectPlay compound address into a DPAddress instance.
   */
  static DPAddress* parse (void* data, DWORD size);

  /**
   * Create a new TCP/IP DPAddress for the given IP. Shorthand method.
   */
  static DPAddress* ip (string ip = "");
};

}
