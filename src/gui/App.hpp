#pragma once
#include <wx/wx.h>
#include "../irc/IRC.hpp"

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
