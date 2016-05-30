#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

using std::string;
using std::vector;

namespace aocmultiny {
namespace irc {

class IRC;

typedef std::function<void(IRC*, vector<string>)> Handler;

class Message {
public:
  Message (string command, string prefix, vector<string> params);

  string command;
  string prefix;
  vector<string> params;
};

class IRC {
private:
  SOCKET socket;
  string host;
  int port;
  int thread;
  std::map<string, vector<Handler>> handlers;

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

  vector<string> channels;
  void list ();
  void join (string channel);
  void part (string channel);
  void who (string channel);

  void privmsg (string target, string message);
  void action (string target, string message);
  void ctcp (string target, string message);

  void on (string command, Handler handler);
  void on (std::map<string, Handler> handlers);
  void onJoinedChannel (string channel);
};

}
}
