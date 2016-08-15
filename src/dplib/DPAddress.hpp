#pragma once
#include <dplobby.h>
#include <string>
#include <vector>
#include <tuple>

using std::pair;
using std::string;
using std::vector;

namespace dplib {

class DPAddress {
private:
  vector<DPCOMPOUNDADDRESSELEMENT*> elements;
  void* address;
  DWORD addressSize = 0;

public:
  DPAddress ();
  DPAddress (GUID serviceProvider);
  ~DPAddress ();

  DPAddress* add (DPCOMPOUNDADDRESSELEMENT* element);
  DPAddress* add (GUID type, const void* data, DWORD dataSize);
  template<typename Value>
  DPAddress* add (GUID type, Value value);

  pair<void*, DWORD> get (GUID type);

  size_t size ();
  void alloc ();
  pair<void*, DWORD> unwrap ();

  static DPAddress* parse (void* data, DWORD size);
  static DPAddress* ip (string ip = "");
};

}
