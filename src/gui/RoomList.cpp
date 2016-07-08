#include "RoomList.hpp"

using std::string;
using std::to_string;
using std::vector;

using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

wxListItem createListItem (int id, string content) {
  wxListItem item;
  item.SetId(id);
  item.SetText(content);
  return item;
}

RoomList::RoomList (wxWindow* parent)
    :
    wxListView(parent, wxID_ANY) {
  this->setupColumns();
}

wxBEGIN_EVENT_TABLE(RoomList, wxListCtrl)
  EVT_LIST_ITEM_ACTIVATED(ID_JOIN_ROOM, RoomList::onDoJoin)
wxEND_EVENT_TABLE()

void RoomList::setupColumns () {
  this->InsertColumn(0, createListItem(0, "Room"));
  this->InsertColumn(1, createListItem(1, "Players"));
  this->InsertColumn(2, createListItem(2, "Description"));
}

void RoomList::setRooms (vector<Channel*> rooms) {
  this->ClearAll();
  this->setupColumns();
  for (auto room : rooms) {
    auto index = this->InsertItem(0, room->name);
    this->SetItem(index, 1, to_string(room->members_count));
    this->SetItem(index, 2, room->topic);
  }
}

void RoomList::onDoJoin (wxListEvent& event) {
  std::wcout << "[RoomList::onDoJoin] " << event.GetText() << std::endl;
}

}
}
