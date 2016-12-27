#pragma once
#include "irc/IRC.hpp"
#include <wx/wx.h>

using aocmultiny::irc::IRC;

namespace aocmultiny {

class App: public wxApp {
public:
  IRC* irc;
  virtual void OnInitCmdLine (wxCmdLineParser& parser);
  virtual bool OnCmdLineParsed (wxCmdLineParser& parser);

  virtual bool OnInit ();
  virtual int OnExit ();
};

}
