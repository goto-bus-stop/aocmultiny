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
  frame->Show(true);

  return true;
}

}
}
