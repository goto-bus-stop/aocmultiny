#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "CLI.hpp"
#include "../irc/IRC.hpp"

using std::to_string;
using std::string;
using std::wstring;
using std::stringstream;
using std::for_each;
using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace cli {

CLI::CLI (IRC* irc)
    :
    irc(irc),
    current_room("") {
}

void CLI::println (string line) {
  std::cout << line << std::endl;
}

void CLI::start () {
  string line;
  this->println("Your nickname?");
  std::getline(std::cin, line);

  this->irc->nick(line);
  this->irc->user(line);

  this->irc->on("323", [this] (IRC* irc, vector<string> params) {
    stringstream room_names ("Rooms:");
    for_each(irc->channels.begin(), irc->channels.end(), [&room_names] (string channel) {
      room_names << channel.substr(1) << " ";
    });
    this->println(room_names.str());
  });

  this->irc->on("JOIN", [this] (IRC* irc, vector<string> params) {
    if (params[1] == "") {
      string room_name = params[0].substr(1);
      this->current_room = room_name;
      this->println("Joined room \"" + room_name + "\"");
    } else {
      this->println("> " + params[1] + " joined the room");
    }
  });
  this->irc->on("PART", [this] (IRC* irc, vector<string> params) {
    if (params.size() == 1 || params[1] == "") {
      this->println("Left room \"" + params[0].substr(1) + "\"");
    } else {
      this->println("> " + params[1] + " left the room");
    }
  });

  while (getline(std::cin, line)) {
    if (line == "list") {
      irc->list();
    } else if (line == "exit") {
      irc->disconnect();
      break;
    } else if (line.substr(0, 5) == "join ") {
      if (this->current_room != "") {
        irc->part("#" + this->current_room);
      }
      string room = line.substr(5);
      irc->join("#" + room);
    }
  }
}

}
}
