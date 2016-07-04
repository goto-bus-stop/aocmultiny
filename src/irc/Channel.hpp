#pragma once
#include <string>
#include <vector>

using namespace std;

namespace aocmultiny {
namespace irc {

class IRC;
class Channel;
class Message;

class Channel {
private:
  IRC* irc;

public:
  Channel (IRC* irc, string name);
  Channel (IRC* irc, string name, int members_count, string topic);

  bool is_virtual;
  string name;
  int members_count;
  string topic;
  vector<string> members;

  void join ();
  void part ();

  void onJoin (string nickname);
  void onPart (string nickname);
};

}
}
