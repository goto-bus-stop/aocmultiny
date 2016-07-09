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

  auto frame = new MainFrame("AoCMulTiny", this->irc);

  wxTextEntryDialog dialog (frame, wxT("Username"));

  dialog.ShowModal();
  auto username = dialog.GetValue().ToStdString();
  std::cout << "Username: " << username << std::endl;

  this->irc->nick(username);
  this->irc->user(username);

  frame->Show(true);

  this->irc->onChannelList += [frame] (auto channels) {
    frame->setRooms(channels);
  };

  this->irc->list();

  return true;
}

int App::OnExit () {
  delete this->irc;
  return 0;
}

}
}
