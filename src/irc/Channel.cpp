#include <algorithm>
#include <string>
#include <vector>
#include "IRC.hpp"
#include "Channel.hpp"

using namespace std;

namespace aocmultiny {
namespace irc {

Channel::Channel (IRC* irc, string name)
    :
    irc(irc),
    is_virtual(true),
    name(name),
    members_count(0),
    topic("") {
}

Channel::Channel (IRC* irc, string name, int members_count, string topic)
    :
    irc(irc),
    is_virtual(false),
    name(name),
    members_count(members_count),
    topic(topic) {
}

void Channel::join () {
  this->irc->join(this->name);
}

void Channel::part () {
  this->irc->part(this->name);
}

void Channel::onJoin (string nickname) {
  this->members.push_back(nickname);
}

void Channel::onPart (string nickname) {
  auto members = this->members;
  auto removed_start = find(members.begin(), members.end(), nickname);
  members.erase(removed_start, members.end());
}

}
}
