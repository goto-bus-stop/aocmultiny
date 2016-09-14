#include "main.hpp"
#include "cli/CLI.hpp"
#include "irc/IRC.hpp"
#include "gui/App.hpp"
#include <dplib/DPAddress.hpp>
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

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

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
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    auto session = DPLobby::get()->hostSession(game, address);
    session
      ->setPlayerName("Host")
      ->launch();
  } else if (action == "join") {
    GUID sessionId; IIDFromString(stow(params[1]).c_str(), &sessionId);
    wcout << "[main] Joining " << to_wstring(sessionId) << endl;
    auto game = new dplib::DPGameAoC();
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    // TODO add in address stuff.
    auto session = DPLobby::get()->joinSession(game, address, sessionId);
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
