#pragma once
#include <string>
#include "../irc/IRC.hpp"

using namespace aocmultiny;

namespace aocmultiny {
namespace cli {

class CLI {
private:
  irc::IRC* irc;
  std::string current_room;
public:
  CLI (irc::IRC* irc);

  void println (std::string line);
  void start ();
};

}
}
