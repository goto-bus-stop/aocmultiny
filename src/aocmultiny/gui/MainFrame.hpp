#pragma once
#include "../irc/IRC.hpp"
#include "../irc/Channel.hpp"
#include "Chat.hpp"
#include "RoomList.hpp"
#include <vector>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/timer.h>

using aocmultiny::irc::IRC;
using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

class MainFrame: public wxFrame {
public:
  MainFrame (const wxString& title, IRC* irc);
  void setRooms (std::vector<Channel*> rooms);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  IRC* irc;
  RoomList* roomList;
  Chat* chat;
  wxTimer* timer;

  void onTimer (wxTimerEvent& event);
  void onHello (wxCommandEvent& event);
  void onExit (wxCommandEvent& event);
  void onAbout (wxCommandEvent& event);
  void onJoinRoom (wxCommandEvent& event);
};

}
}
