#include "Chat.hpp"
#include <iostream>

using std::string;
using std::to_string;
using std::vector;
using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

wxIMPLEMENT_DYNAMIC_CLASS(Chat, wxListView)

Chat::Chat () {
}

Chat::Chat (wxWindow* parent, IRC* irc, string channelName)
    :
    wxListView(parent, wxID_ANY),
    irc(irc),
    channelName(channelName) {
  this->InsertColumn(0, "User");
  this->InsertColumn(1, "");

  this->listener = this->irc->on("PRIVMSG", [this] (auto, auto message) {
    if (message->params.size() < 2) {
      return;
    }

    if (message->params[0] == this->channelName) {
      this->add(message->prefix->nickname, message->params[1]);
    }
  });
}

Chat::~Chat () {
  this->irc->off("PRIVMSG", this->listener);
}

void Chat::add (string user, string message) {
  auto index = this->InsertItem(this->GetItemCount(), user);
  this->SetItem(index, 1, message);
}

void Chat::setChannel (string channel) {
  this->channelName = channel;
}

}
}
