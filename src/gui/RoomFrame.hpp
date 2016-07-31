#pragma once
#include "../irc/IRC.hpp"
#include "Chat.hpp"
#include <wx/wx.h>
#include <string>

using std::string;
using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

class RoomFrame: public wxFrame {
public:
  RoomFrame (wxWindow* parent, IRC* irc, string roomName);
  void setRoom(string name);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  string channelName;
  IRC* irc;
  Chat* chat;

  void onSize (wxSizeEvent& event);
  void onClose (wxCloseEvent& event);
};

}
}
