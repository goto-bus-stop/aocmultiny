#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <process.h>
#include "IRC.hpp"

using std::string;
using std::vector;

namespace aocmultiny {
namespace irc {

// https://tools.ietf.org/html/rfc1459#section-2.3
const int MESSAGE_SIZE = 512;

typedef std::function<void(IRC*, vector<string>)> Handler;

static string RPL_LISTSTART = "321";
static string RPL_LIST = "322";
static string RPL_LISTEND = "323";

const std::map<string, Handler> handlers = {
  { "JOIN", [] (IRC* irc, vector<string> params) {
    irc->onJoinedChannel(params[0]);
  } },
  { RPL_LISTSTART, [] (IRC* irc, vector<string> params) {
    irc->onListStart();
  } },
  { RPL_LIST, [] (IRC* irc, vector<string> params) {
    irc->onListEntry(params[1]);
  } },
  { RPL_LISTEND, [] (IRC* irc, vector<string> params) {
    irc->onListEnd();
  } },
};

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
  this->initWinSock();
  this->connect();
}

IRC::~IRC () {
  this->disconnect();
}

void IRC::initWinSock () {
  WSADATA wsaData;
  std::wcout << "[IRC::initWinSock] WSAStartup" << std::endl;
  int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result) {
    std::wcout << ":(" << std::endl;
    return;
  }
}

void IRC::connect () {
  const char* hostname = this->host.c_str();
  const char* port = std::to_string(this->port).c_str();
  struct addrinfo* address = NULL;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  std::wcout << "[IRC::connect] getaddrinfo " << hostname << ":" << port << std::endl;
  int result = getaddrinfo(hostname, port, &hints, &address);
  if (result) {
    std::wcout << "[IRC::connect] Invalid host" << std::endl;
    return;
  }

  this->socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
  if (this->socket == INVALID_SOCKET) {
    std::wcout << "[IRC::connect] Could not connect to host" << std::endl;
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
  std::cout << "[IRC::send] " << message << std::endl;
  int result = ::send(this->socket, line.c_str(), line.size(), 0);
  if (!result) {
    std::wcout << result << std::endl;
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
      while ((pos2 = raw.find(" ", pos1)) != std::string::npos) {
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
    const auto& handler = handlers.at(message.command);
    if (handler != NULL) {
      handler(this, message.params);
    }
  } catch (std::out_of_range) {
    std::cout << "[IRC::execute] Unrecognized command: " << message.command << std::endl;
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

void IRC::onJoinedChannel (string channel) {
  std::cout << "[IRC::onJoinedChannel] " << channel << std::endl;
}

void IRC::onListStart () {
  std::cout << "[IRC::onListStart]" << std::endl;
  this->next_channels = vector<string> ();
}
void IRC::onListEntry (string channel) {
  std::cout << "[IRC::onListEntry] " << channel << std::endl;
  this->next_channels.push_back(channel);
}
void IRC::onListEnd () {
  std::cout << "[IRC::onListEnd]" << std::endl;
  this->channels = this->next_channels;
}

}
}
