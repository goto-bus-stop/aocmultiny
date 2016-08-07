#include "main.hpp"
#include "cli/CLI.hpp"
#include "irc/IRC.hpp"
#include "gui/App.hpp"
#include <dplib/DPLobby.hpp>
#include <dplib/DPGameAoC.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wx/app.h>

using namespace std;
using dplib::DPLobby;

namespace aocmultiny {

static wstring stow (string s) {
  wstring w;
  for (auto c : s) {
    w.push_back(c);
  }
  return w;
}

void main (vector<string> params) {
  CoInitialize(NULL);
  wcout << "[main] Starting" << endl;

  auto action = params.size() > 0 ? params[0] : "";
  if (action == "host") {
    wcout << "[main] Hosting" << endl;
    auto game = new dplib::DPGameAoC();
    auto session = DPLobby::get()->hostSession(game);
    session
      ->setPlayerName("Host")
      ->launch();
  } else if (action == "join") {
    GUID sessionId; IIDFromString(stow(params[2]).c_str(), &sessionId);
    wcout << "[main] Joining " << to_wstring(sessionId) << endl;
    auto game = new dplib::DPGameAoC();
    // TODO add in address stuff.
    auto session = DPLobby::get()->joinSession(game, NULL, sessionId);
    session
      ->setPlayerName("Rando")
      ->launch();
  } else if (action == "cli") {
    wcout << "[main] IRC" << endl;
    auto irc = new irc::IRC("localhost");
    auto cli = new cli::CLI(irc);
    cli->start();
    delete irc;
  } else {
    wxEntry(0, nullptr);
  }

  wcout << "[main] Exiting" << endl;

  CoUninitialize();
}

}

wxIMPLEMENT_APP_NO_MAIN(aocmultiny::gui::App);
IMPLEMENT_WX_THEME_SUPPORT;

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {
  vector<string> params = split(cmdLine, ' ');

  aocmultiny::main(params);
  return 0;
}
