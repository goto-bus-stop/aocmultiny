#include "common.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wx/app.h>
#include "main.hpp"
#include "dplib/DPLobby.hpp"
#include "dplib/DPGameAoC.hpp"
#include "cli/CLI.hpp"
#include "irc/IRC.hpp"
#include "gui/App.hpp"

using namespace std;

namespace aocmultiny {

void main (vector<string> params) {
  CoInitialize(NULL);
  wcout << "[main] Starting" << endl;

  auto action = params.size() > 0 ? params[0] : "";
  if (action == "host") {
    auto lobby = new dplib::DPLobby(new dplib::DPGameAoC(), "Hosting");
    lobby->host();
    lobby->launch();
  } else if (action == "join") {
    auto lobby = new dplib::DPLobby(new dplib::DPGameAoC(), "Joining");
    lobby->join({ 0 }, params[1]);
    lobby->launch();
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
