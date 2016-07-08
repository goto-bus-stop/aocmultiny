#pragma once
#include <vector>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../irc/Channel.hpp"
#include "RoomList.hpp"

using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

class MainFrame: public wxFrame {
public:
  MainFrame (const wxString& title);
  void setRooms (std::vector<Channel*> rooms);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  RoomList* roomList;

  void onHello (wxCommandEvent& event);
  void onExit (wxCommandEvent& event);
  void onAbout (wxCommandEvent& event);
};

}
}
