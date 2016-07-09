#pragma once
#include <vector>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../irc/Channel.hpp"

using std::vector;
using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

enum {
  ID_JOIN_ROOM
};

wxDECLARE_EVENT(JOIN_ROOM, wxCommandEvent);

class RoomList: public wxListView {
  wxDECLARE_DYNAMIC_CLASS(RoomList);
public:
  RoomList ();
  RoomList (wxWindow* parent);
  void setRooms (vector<Channel*> rooms);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  void setupColumns ();
  void onDoJoin (wxListEvent& event);
};

}
}
