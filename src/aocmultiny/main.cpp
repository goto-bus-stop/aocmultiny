#include "main.hpp"
#include "cli/CLI.hpp"
#include "irc/IRC.hpp"
#include "gui/MainFrame.hpp"
#include <dplib/DPAddress.hpp>
#include <dplib/DPLobby.hpp>
#include <dplib/DPGameAoC.hpp>
#include <string>
#include <iostream>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/socket.h>
#include <wx/utils.h>
#include "patches/HDCompat.hpp"
#include "patches/UserPatch.hpp"

using namespace std;
using dplib::DPLobby;
using aocmultiny::gui::MainFrame;

namespace aocmultiny {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

void App::OnInitCmdLine (wxCmdLineParser& parser) {
  wxApp::OnInitCmdLine(parser);

  parser.AddLongOption("player", "The local player name. Defaults to the current operating system username.");
  parser.AddLongSwitch("host", "Host a game. (Headless)");
  parser.AddLongOption("join", "Join a game. (Headless)");
  parser.AddSwitch("hd", "hdcompat", "Attempt to install the aoccs.net HD Edition compatibility patch.");
  parser.AddSwitch("up", "userpatch", "Attempt to install UserPatch.");
}

bool App::OnCmdLineParsed (wxCmdLineParser& parser) {
  if (parser.Found("hd")) {
    std::cout << "Installing HD Compat Patch" << std::endl;
    auto hd = new aocmultiny::patches::HDCompat();
    hd->install();
  }
  if (parser.Found("up")) {
    std::cout << "Installing UserPatch" << std::endl;
    auto up = new aocmultiny::patches::UserPatch();
    up->install();
  }

  if (parser.Found("hd") || parser.Found("up")) {
    return false;
  }

  // Try to read the --player flag, or default to the current user.
  wxString wxPlayerName = wxGetUserId();
  parser.Found("player", &wxPlayerName);
  string playerName = wxPlayerName.ToStdString();

  if (parser.Found("host")) {
    wcout << "[main] Hosting as " << playerName << endl;
    auto game = new dplib::DPGameAoC();
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    auto session = DPLobby::get()->hostSession(game, address);
    session
      ->setPlayerName(playerName)
      ->launch();
    return false;
  } else if (parser.Found("join")) {
    // Bit roundabout: wxString → wchar_t* → GUID.
    wxString sessionStr;
    GUID sessionId;
    parser.Found("join", &sessionStr);
    IIDFromString(sessionStr.c_str(), &sessionId);

    wcout << "[main] Joining " << to_wstring(sessionId) << " as " << playerName << endl;
    auto game = new dplib::DPGameAoC();
    auto address = new dplib::DPAddress(DPSPGUID_NICE);
    // TODO add in address stuff.
    auto session = DPLobby::get()->joinSession(game, address, sessionId);
    session
      ->setPlayerName(playerName)
      ->launch();
    return false;
  }

  return wxApp::OnCmdLineParsed(parser);
}

bool App::OnInit () {
  CoInitialize(NULL);
  this->irc = nullptr;

  wxSocketBase::Initialize();

  wcout << "[main] Starting" << endl;

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
  if (this->irc) {
    delete this->irc;
  }

  wxSocketBase::Shutdown();

  CoUninitialize();

  return 0;
}

}

wxIMPLEMENT_APP(aocmultiny::App);
IMPLEMENT_WX_THEME_SUPPORT;
