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

class RoomList: public wxListView {
public:
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
