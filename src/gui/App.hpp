#pragma once
#include <wx/app.h>
#include "../irc/IRC.hpp"

using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

class App: public wxApp {
public:
  IRC* irc;
  virtual bool OnInit ();
};

}
}
