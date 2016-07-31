#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Channel.hpp"
#include "../util.hpp"
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <utility>

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace aocmultiny {
namespace irc {

class IRC;
class Channel;
class Message;
class MessagePrefix;

typedef std::function<void(IRC*, Message*)> Handler;
typedef vector<pair<int, Handler>> HandlerList;
typedef vector<Channel*> ChannelList;

class MessagePrefix {
public:
  MessagePrefix (string nickname, string username, string hostname);

  string nickname;
  string username;
  string hostname;
};

class Message {
public:
  Message (string command, MessagePrefix* prefix, vector<string> params);
  ~Message ();

  string command;
  MessagePrefix* prefix;
  vector<string> params;
};

class IRC {
private:
  SOCKET socket;
  string buffer;
  string host;
  int port;
  int thread;
  map<string, HandlerList> handlers;
  int handlerIdx;
  ChannelList next_channels;

  void attachDefaultHandlers ();
  void initWinSock ();
  void send (string message);
  Message* parse (string raw);
public:
  IRC (string host, int port = 6667);
  ~IRC ();

  void startReceiveThread ();
  void receive ();
  bool running;

  void execute (Message* message);

  void connect ();
  void disconnect ();
  void nick (string nick);
  void user (string username, string hostname, string servername, string realname);
  void user (string username, string realname);
  void user (string username);

  ChannelList channels;
  Channel* channel (string name);
  void list ();
  void join (string channel);
  void part (string channel);
  void who (string channel);

  void privmsg (string target, string message);
  void action (string target, string message);
  void ctcp (string target, string message);
  bool is_ctcp (string message);

  int on (string command, Handler handler);
  void off (int listener);
  void off (string command, int listener);
  void onJoinedChannel (string channel);

  EventListeners<ChannelList> onChannelList;
};

}
}
