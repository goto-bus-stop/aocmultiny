#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Channel.hpp"
#include "../util.hpp"
#include <functional>
#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;

namespace aocmultiny {
namespace irc {

class IRC;
class Channel;
class Message;
class MessagePrefix;

typedef std::function<void(IRC*, vector<string>)> Handler;
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
  std::map<string, vector<Handler>> handlers;
  vector<Channel*> next_channels;

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

  void execute (Message message);

  void connect ();
  void disconnect ();
  void nick (string nick);
  void user (string username, string hostname, string servername, string realname);
  void user (string username, string realname);
  void user (string username);

  vector<Channel*> channels;
  Channel* channel (string name);
  void list ();
  void join (string channel);
  void part (string channel);
  void who (string channel);

  void privmsg (string target, string message);
  void action (string target, string message);
  void ctcp (string target, string message);
  bool is_ctcp (string message);

  void on (string command, Handler handler);
  void on (std::map<string, Handler> handlers);
  void onJoinedChannel (string channel);

  EventListeners<ChannelList> onChannelList;
};

}
}
