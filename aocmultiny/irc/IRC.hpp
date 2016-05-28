#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

using std::string;
using std::vector;

namespace aocmultiny {
namespace irc {

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
  vector<string> channels;
  vector<string> next_channels;
  int thread;

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
  void list ();
  void join (string channel);
  void part (string channel);

  void onJoinedChannel (string channel);
  void onListStart ();
  void onListEntry (string channel);
  void onListEnd ();
};

}
}
