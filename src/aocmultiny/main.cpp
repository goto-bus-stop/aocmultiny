#include "main.hpp"
#include "cli/CLI.hpp"
#include "irc/IRC.hpp"
#include "gui/MainFrame.hpp"
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
using aocmultiny::gui::MainFrame;

namespace aocmultiny {

static wstring stow (string s) {
  wstring w;
  for (auto c : s) {
    w.push_back(c);
  }
  return w;
}

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

bool App::OnInit () {
  CoInitialize(NULL);

  wcout << "[main] Starting" << endl;
  vector<string> params = split(GetCommandLineA(), ' ');

  auto action = params.size() > 1 ? params[1] : "";
  if (action == "host") {
    wcout << "[main] Hosting" << endl;
    auto game = new dplib::DPGameAoC();
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    auto session = DPLobby::get()->hostSession(game, address);
    session
      ->setPlayerName("Host")
      ->launch();
    return false;
  } else if (action == "join") {
    GUID sessionId; IIDFromString(stow(params[2]).c_str(), &sessionId);
    wcout << "[main] Joining " << to_wstring(sessionId) << endl;
    auto game = new dplib::DPGameAoC();
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    // TODO add in address stuff.
    auto session = DPLobby::get()->joinSession(game, address, sessionId);
    session
      ->setPlayerName("Rando")
      ->launch();
    return false;
  } else if (action == "cli") {
    wcout << "[main] IRC" << endl;
    auto irc = new irc::IRC("localhost");
    auto cli = new cli::CLI(irc);
    cli->start();
    delete irc;
    return false;
  }

  if (!wxApp::OnInit()) {
    return false;
  }

  wxTextEntryDialog dialog (NULL, wxT("Username"));
  if (dialog.ShowModal() == wxID_CANCEL) {
    return false;
  }

  auto username = dialog.GetValue().ToStdString();
  this->irc = new irc::IRC("localhost");
  auto frame = new MainFrame("AoCMulTiny", this->irc);
  this->irc->nick(username);
  this->irc->user(username);
  frame->Show(true);
  this->irc->join("#lobby");

  return true;
}

int App::OnExit () {
  delete this->irc;

  CoUninitialize();

  return 0;
}

}

wxIMPLEMENT_APP(aocmultiny::App);
IMPLEMENT_WX_THEME_SUPPORT;
