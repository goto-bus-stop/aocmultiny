#pragma once
#include <string>
#include <dplobby.h>

namespace aocmultiny {
namespace dplib {

class DPAddress {

private:
  LPDIRECTPLAYLOBBY3A lobby;
  std::string ip;

public:
  void* address;
  int size;

  DPAddress (LPDIRECTPLAYLOBBY3A lobby, std::string ip);
  void alloc ();
  void* unwrap ();

};

}
}
