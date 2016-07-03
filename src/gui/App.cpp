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

  auto frame = new MainFrame("AoCMulTiny");

  wxTextEntryDialog dialog (frame, wxT("Username"));

  dialog.ShowModal();
  auto username = dialog.GetValue().ToStdString();
  std::cout << "Username: " << username << std::endl;

  this->irc->nick(username);
  this->irc->user(username);

  frame->Show(true);

  this->irc->on("323", [frame] (IRC* irc, vector<string> params) {
    frame->setRooms(irc->channels);
  });

  this->irc->list();

  return true;
}

int App::OnExit () {
  delete this->irc;
  return 0;
}

}
}
