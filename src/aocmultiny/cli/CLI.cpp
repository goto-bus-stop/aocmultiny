#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "CLI.hpp"
#include "../irc/IRC.hpp"
#include "../../util.hpp"
#include <dplib/DPLobby.hpp>
#include <dplib/DPGameAoC.hpp>

using std::to_string;
using std::string;
using std::wstring;
using std::stringstream;
using std::for_each;
using dplib::DPLobby;
using dplib::DPGameAoC;
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
  string player_name;
  this->println("Your nickname?");
  std::getline(std::cin, player_name);

  this->irc->nick(player_name);
  this->irc->user(player_name);

  auto game = new DPGameAoC();
  auto lobby = DPLobby::get()->setGame(game)->setPlayerName(player_name);

  this->irc->on("323", [this] (auto irc, auto message) {
    stringstream room_names ("Rooms:");
    for (auto channel : irc->channels) {
      room_names << channel->name.substr(1) << " ";
    }
    this->println(room_names.str());
  });

  this->irc->on("JOIN", [this] (auto irc, auto message) {
    auto params = message->params;
    if (params.size() == 1 || params[1] == "") {
      auto room_name = params[0].substr(1);
      this->current_room = room_name;
      this->println("Joined room \"" + room_name + "\"");
    } else {
      this->println("> " + params[1] + " joined the room");
    }
  });
  this->irc->on("PART", [this] (auto irc, auto message) {
    auto params = message->params;
    if (params.size() == 1 || params[1] == "") {
      this->println("Left room \"" + params[0].substr(1) + "\"");
    } else {
      this->println("> " + params[1] + " left the room");
    }
  });

  this->irc->on("PRIVMSG", [this, lobby] (auto irc, auto message) {
    auto action = message->params.back();
    if (this->irc->is_ctcp(action)) {
      auto ctcp = split(action.substr(1, -1), ' ');
      if (ctcp[0] == "AOC_START" && ctcp.size() >= 3) {
        wstring sessionStr;
        for (const char c : ctcp[1]) {
          sessionStr.push_back(static_cast<wchar_t>(c));
        }
        GUID sessionId; IIDFromString(sessionStr.c_str(), &sessionId);
        lobby->join(sessionId, ctcp[2]);
        lobby->launch();
      }
    }
  });

  string line;
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
    } else if (line == "start") {
      if (this->current_room == "") {
        this->println("Join a room first before starting a game.");
        continue;
      }
      lobby->host();
      auto guid = lobby->getSessionGUID();
      this->println("Launching session " + to_string(guid));

      lobby->onConnectSucceeded += [this, guid] () {
        this->irc->ctcp("#" + this->current_room, "AOC_START " + to_string(guid));
      };
      lobby->launch();
    }
  }
}

}
}
