#pragma once
#include "../irc/IRC.hpp"
#include <wx/wx.h>

using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

class App: public wxApp {
public:
  IRC* irc;
  virtual bool OnInit ();
  virtual int OnExit ();
};

}
}
