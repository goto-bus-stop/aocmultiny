#include "../irc/IRC.hpp"
#include "RoomFrame.hpp"
#include "Chat.hpp"
#include <string>

using std::string;
using aocmultiny::irc::IRC;

namespace aocmultiny {
namespace gui {

RoomFrame::RoomFrame (wxWindow* parent, IRC* irc, string channelName)
    :
    wxFrame(parent, wxID_ANY, channelName),
    channelName(channelName),
    irc(irc) {
  this->irc->join(this->channelName);
  this->chat = new Chat(this, irc, this->channelName);
}

void RoomFrame::setRoom (string channelName) {
  this->chat->setChannel(channelName);
}

void RoomFrame::onSize (wxSizeEvent& event) {
  if (this->GetAutoLayout()) {
    this->Layout();
  }
}

void RoomFrame::onClose (wxCloseEvent& event) {
  this->Hide();
  this->irc->part(this->channelName);
}

wxBEGIN_EVENT_TABLE(RoomFrame, wxFrame)
  EVT_CLOSE(RoomFrame::onClose)
  EVT_SIZE(RoomFrame::onSize)
wxEND_EVENT_TABLE()

}
}
