#include "IRC.hpp"
#include "Channel.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <process.h>

using namespace std;

namespace aocmultiny {
namespace irc {

// https://tools.ietf.org/html/rfc1459#section-2.3
const int MESSAGE_SIZE = 512;

static string RPL_LISTSTART = "321";
static string RPL_LIST = "322";
static string RPL_LISTEND = "323";

Message::Message (string command, MessagePrefix* prefix, vector<string> params)
    :
    command(command),
    prefix(prefix),
    params(params) {
}

Message::~Message () {
  delete this->prefix;
}

MessagePrefix::MessagePrefix (string nickname, string username, string hostname)
    :
    nickname(nickname),
    username(username),
    hostname(hostname)
    {
}

void receiveThread (void* data) {
  auto client = static_cast<IRC*>(data);
  while (client->running) {
    client->receive();
  }
  _endthread();
}

IRC::IRC (string host, int port)
    :
    socket(INVALID_SOCKET),
    host(host),
    port(port),
    thread(0),
    running(false),
    channels(vector<Channel*> ()) {
  this->attachDefaultHandlers();
  this->initWinSock();
  this->connect();
}

IRC::~IRC () {
  if (this->running) {
    this->disconnect();
  }
}

void IRC::attachDefaultHandlers () {
  this->on("JOIN", [this] (auto irc, auto params) {
    this->onJoinedChannel(params[0]);
  });
  this->on(RPL_LISTSTART, [this] (auto irc, auto params) {
    this->next_channels.clear();
  });
  this->on(RPL_LIST, [this] (auto irc, auto params) {
    auto members_count = params.size() > 2 ? stoi(params[2]) : 0;
    auto topic = params.size() > 3 ? params[3] : "";
    this->next_channels.push_back(
      new Channel(irc, params[1], members_count, topic)
    );
  });
  this->on(RPL_LISTEND, [this] (auto, auto) {
    auto prev_channels = this->channels;
    this->channels = this->next_channels;
    for (auto channel : prev_channels) {
      delete channel;
    }
    this->next_channels.clear();
    this->onChannelList.emit(this->channels);
  });
}

void IRC::initWinSock () {
  WSADATA wsaData;
  wcout << "[IRC::initWinSock] WSAStartup" << endl;
  auto result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result) {
    wcout << ":(" << endl;
    return;
  }
}

void IRC::connect () {
  const auto hostname = this->host.c_str();
  const auto port = to_string(this->port).c_str();
  struct addrinfo* address = NULL;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  wcout << "[IRC::connect] getaddrinfo " << hostname << ":" << port << endl;
  auto result = getaddrinfo(hostname, port, &hints, &address);
  if (result) {
    wcout << "[IRC::connect] Invalid host" << endl;
    return;
  }

  this->socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
  if (this->socket == INVALID_SOCKET) {
    wcout << "[IRC::connect] Could not connect to host" << endl;
    return;
  }
  result = ::connect(this->socket, address->ai_addr, static_cast<int>(address->ai_addrlen));
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
  auto line = message + "\r\n";
  cout << "[IRC::send] " << message << endl;
  auto result = ::send(this->socket, line.c_str(), line.size(), 0);
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
  auto length = ::recv(this->socket, buffer, MESSAGE_SIZE, 0);
  if (length == SOCKET_ERROR) {
    return;
  }
  auto lines = this->buffer + buffer;
  this->buffer = "";
  istringstream stream (lines);
  string line;
  while (getline(stream, line)) {
    // IRC uses \r\n, so trim final \r
    if (line.find("\r") == string::npos) {
      this->buffer = line;
      return;
    } else {
      line = line.substr(0, line.size() - 1);
    }
    auto message = this->parse(line);
    this->execute(*message);
  }
}

MessagePrefix* parsePrefix (string raw) {
  string nickname = "";
  string username = "";
  string hostname = "";
  auto split = raw.find("!");
  if (split != string::npos) {
    nickname = raw.substr(0, split);
    raw = raw.substr(split + 1);
  }
  split = raw.find("@");
  if (split != string::npos) {
    username = raw.substr(0, split);
    raw = raw.substr(split + 1);
  }
  hostname = raw;
  return new MessagePrefix(nickname, username, hostname);
}

Message* IRC::parse (string raw) {
  string prefix = "";
  string command = "";
  vector<string> parameters;

  cout << "[IRC::parse] " << raw << endl;

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

  return new Message(command, parsePrefix(prefix), parameters);
}

void IRC::execute (Message message) {
  try {
    const auto& handlers = this->handlers.at(message.command);
    cout << "[IRC::execute] Handlers for " << message.command << endl;
    for (auto handler : handlers) {
      handler(this, message.params);
    }
  } catch (std::out_of_range) {
    cout << "[IRC::execute] Unrecognized command: " << message.command << endl;
  }
}

void IRC::nick (string nick) {
  this->send("NICK " + nick);
}

void IRC::user (string username, string hostname, string servername, string realname) {
  this->send("USER " + username + " " + hostname + " " + servername + " :" + realname);
}

void IRC::user (string username, string realname) {
  this->user(username, username, this->host, realname);
}

void IRC::user (string username) {
  this->user(username, username);
}

void IRC::list () {
  this->send("LIST");
}

Channel* IRC::channel (string name) {
  for (auto channel : this->channels) {
    if (channel->name == name) {
      return channel;
    }
  }

  return new Channel(this, name);
}

void IRC::join (string channel) {
  this->send("JOIN " + channel);
}

void IRC::part (string channel) {
  this->send("PART " + channel);
}

void IRC::who (string channel) {
  this->send("WHO " + channel);
}

void IRC::privmsg (string target, string message) {
  this->send("PRIVMSG " + target + " :" + message);
}

void IRC::ctcp (string target, string message) {
  this->privmsg(target, "\u0001" + message + "\u0001");
}

bool IRC::is_ctcp (string message) {
  return message.front() == '\u0001' && message.back() == '\u0001';
}

void IRC::action (string target, string message) {
  this->ctcp(target, "ACTION " + message);
}

void IRC::on (string command, Handler handler) {
  try {
    this->handlers[command].push_back(handler);
  } catch (std::out_of_range) {
    this->handlers[command] = { handler };
  }
}

void IRC::on (map<string, Handler> events) {
  for (auto entry : events) {
    this->on(entry.first, entry.second);
  }
}

void IRC::onJoinedChannel (string channel) {
  cout << "[IRC::onJoinedChannel] " << channel << endl;
}

}
}
