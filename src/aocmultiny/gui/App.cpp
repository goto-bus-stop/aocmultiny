#include "App.hpp"
#include "MainFrame.hpp"

using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

bool App::OnInit () {
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
  return 0;
}

}
}
