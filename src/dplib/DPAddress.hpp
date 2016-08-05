#pragma once
#include <string>
#include <vector>
#include <dplobby.h>

using std::string;
using std::vector;

namespace dplib {

class DPAddress {
private:
  vector<DPCOMPOUNDADDRESSELEMENT*> elements;

public:
  DPAddress ();
  DPAddress (GUID serviceProvider);

  DPAddress* add (DPCOMPOUNDADDRESSELEMENT* element);
  DPAddress* add (GUID type, void* data, int dataSize);

  size_t size ();
  DPCOMPOUNDADDRESSELEMENT* alloc ();

  static DPAddress* ip (string ip = "");
};

}
