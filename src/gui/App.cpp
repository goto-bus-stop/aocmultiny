#include "App.hpp"
#include "MainFrame.hpp"

using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

bool App::OnInit () {
  if (!wxApp::OnInit()) {
    return false;
  }

  this->irc = new irc::IRC("localhost");

  wxFrame* frame = new MainFrame("AoCMulTiny");

  wxTextEntryDialog dialog (frame, wxT("Username"));

  dialog.ShowModal();
  string username = dialog.GetValue().ToStdString();
  std::cout << "Username: " << username << std::endl;

  this->irc->nick(username);
  this->irc->user(username);

  frame->Show(true);

  return true;
}

int App::OnExit () {
  delete this->irc;
  return 0;
}

}
}
