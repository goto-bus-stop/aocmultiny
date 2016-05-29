#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <process.h>
#include "IRC.hpp"

using namespace std;

namespace aocmultiny {
namespace irc {

// https://tools.ietf.org/html/rfc1459#section-2.3
const int MESSAGE_SIZE = 512;

static string RPL_LISTSTART = "321";
static string RPL_LIST = "322";
static string RPL_LISTEND = "323";

Message::Message (string command, string prefix, vector<string> params)
    :
    command(command),
    prefix(prefix),
    params(params) {
}

void receiveThread (void* client) {
  while (((IRC*) client)->running) {
    ((IRC*) client)->receive();
  }
  _endthread();
}

IRC::IRC (string host, int port)
    :
    socket(INVALID_SOCKET),
    host(host),
    port(port),
    channels(vector<string> ()),
    thread(0),
    running(false) {
  this->attachDefaultHandlers();
  this->initWinSock();
  this->connect();
}

IRC::~IRC () {
  this->disconnect();
}

void IRC::attachDefaultHandlers () {
  this->on("JOIN", [this] (IRC* irc, vector<string> params) {
    this->onJoinedChannel(params[0]);
  });
  auto next_channels = new vector<string>();
  this->on(RPL_LISTSTART, [this, &next_channels] (IRC* irc, vector<string> params) {
    next_channels = new vector<string>();
  });
  this->on(RPL_LIST, [this, &next_channels] (IRC* irc, vector<string> params) {
    next_channels->push_back(params[1]);
  });
  this->on(RPL_LISTEND, [this, &next_channels] (IRC* irc, vector<string> params) {
    this->channels = *next_channels;
  });
}

void IRC::initWinSock () {
  WSADATA wsaData;
  wcout << "[IRC::initWinSock] WSAStartup" << endl;
  int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result) {
    wcout << ":(" << endl;
    return;
  }
}

void IRC::connect () {
  const char* hostname = this->host.c_str();
  const char* port = to_string(this->port).c_str();
  struct addrinfo* address = NULL;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  wcout << "[IRC::connect] getaddrinfo " << hostname << ":" << port << endl;
  int result = getaddrinfo(hostname, port, &hints, &address);
  if (result) {
    wcout << "[IRC::connect] Invalid host" << endl;
    return;
  }

  this->socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
  if (this->socket == INVALID_SOCKET) {
    wcout << "[IRC::connect] Could not connect to host" << endl;
    return;
  }
  result = ::connect(this->socket, address->ai_addr, (int) address->ai_addrlen);
  if (result != SOCKET_ERROR) {
    this->startReceiveThread();
  } else {
    ::closesocket(this->socket);
  }

  freeaddrinfo(address);
}

void IRC::disconnect () {
  this->send("QUIT");
  ::closesocket(this->socket);
  this->running = false;
}

void IRC::send (string message) {
  string line = message + "\r\n";
  cout << "[IRC::send] " << message << endl;
  int result = ::send(this->socket, line.c_str(), line.size(), 0);
  if (!result) {
    wcout << result << endl;
  }
}

void IRC::startReceiveThread () {
  this->running = true;
  this->thread = _beginthread(&receiveThread, 0, this);
}

void IRC::receive () {
  char buffer[MESSAGE_SIZE];
  memset(buffer, 0, MESSAGE_SIZE);
  int length = ::recv(this->socket, buffer, MESSAGE_SIZE, 0);
  auto message = this->parse(buffer);
  this->execute(*message);
}

Message* IRC::parse (string raw) {
  string prefix = "";
  string command = "";
  vector<string> parameters;

  if (raw[0] == ':') {
    int end = raw.find(" ");
    prefix = raw.substr(0, end);
    raw = raw.substr(end + 1);
  }

  command = raw.substr(0, raw.find(" "));
  raw = raw.substr(command.length() + 1);

  if (raw != "") {
    if (raw[0] == ':') {
      parameters.push_back(raw.substr(1));
    } else {
      size_t pos1 = 0;
      size_t pos2;
      while ((pos2 = raw.find(" ", pos1)) != string::npos) {
        parameters.push_back(raw.substr(pos1, pos2 - pos1));
        pos1 = pos2 + 1;
        if (raw.substr(pos1, 1) == ":") {
          parameters.push_back(raw.substr(pos1 + 1));
          break;
        }
      }
      if (parameters.empty()) {
        parameters.push_back(raw);
      }
    }
  }

  return new Message(command, prefix, parameters);
}

void IRC::execute (Message message) {
  try {
    const auto& handlers = this->handlers.at(message.command);
    cout << "[IRC::execute] Handlers for " << message.command << endl;
    for_each(handlers.begin(), handlers.end(), [this, &message] (Handler handler) {
      handler(this, message.params);
    });
  } catch (std::out_of_range) {
    cout << "[IRC::execute] Unrecognized command: " << message.command << endl;
  }
}

void IRC::nick (string nick) {
  this->send("NICK " + nick);
}

void IRC::list () {
  this->send("LIST");
}

void IRC::join (string channel) {
  this->send("JOIN " + channel);
}

void IRC::part (string channel) {
  this->send("PART " + channel);
}

void IRC::on (string command, Handler handler) {
  try {
    this->handlers[command].push_back(handler);
  } catch (std::out_of_range) {
    this->handlers[command] = { handler };
  }
}

void IRC::on (map<string, Handler> events) {
  for_each(events.begin(), events.end(), [this] (pair<string, Handler> entry) {
    this->on(entry.first, entry.second);
  });
}

void IRC::onJoinedChannel (string channel) {
  cout << "[IRC::onJoinedChannel] " << channel << endl;
}

}
}
