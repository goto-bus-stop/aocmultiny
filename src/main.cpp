#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "main.hpp"
#include "Lobby.hpp"
#include "irc/IRC.hpp"

using namespace std;

namespace aocmultiny {

void main (vector<string> params) {
  CoInitialize(NULL);
  wcout << "[main] Starting" << endl;

  string action = params.size() > 0 ? params[0] : "";

  if (action == "host") {
    Lobby lobby;
    lobby.host();
  } else if (action == "join") {
    Lobby lobby;
    lobby.join({ 0 }, params[1]);
  } else {
    wcout << "[main] IRC" << endl;
    auto irc = new irc::IRC("localhost");
    irc->nick("AoCMulTiny");
    irc->on({
      { "NICK", [] (irc::IRC* irc, vector<string> params) {
        wcout << "[main] NICK ok" << endl;
      } },
      { "433", [] (irc::IRC* irc, vector<string> params) {
        wcout << "[main] Nickname is in use" << endl;
      } }
    });
    irc->list();
    irc->on("323", [] (irc::IRC* irc, vector<string> params) {
      for_each(irc->channels.begin(), irc->channels.end(), [] (string channel) {
        cout << channel << " ";
      });
      wcout << endl;
    });
    getchar();
    delete irc;
  }

  wcout << "[main] Exiting" << endl;

  CoUninitialize();
}

}

vector<string> split(const string &s, char delim) {
  stringstream ss (s);
  string item;
  vector<std::string> elems;
  while (getline(ss, item, delim)) {
    elems.push_back(move(item));
  }
  return elems;
}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {
  vector<string> params = split(cmdLine, ' ');

  aocmultiny::main(params);
  return 0;
}
