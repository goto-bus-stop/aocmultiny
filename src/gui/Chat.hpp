#pragma once
#include "../irc/IRC.hpp"
#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

using std::string;
using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

class Chat: public wxListView {
  wxDECLARE_DYNAMIC_CLASS(Chat);

private:
  string channelName;
  IRC* irc;
  int listener;

public:
  Chat ();
  Chat (wxWindow* parent, IRC* irc, string channelName);
  ~Chat ();

  void setChannel (string channelName);
  void add (string user, string message);
};

}
}
