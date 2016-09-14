#include "SignalingConnection.hpp"
#include "../util.hpp"
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

using std::string;
using std::to_string;

namespace nicesp {

static void* signalThread (void* data) {
  auto connection = reinterpret_cast<SignalingConnection*>(data);
  gchar* line = nullptr;
  g_message("[SignalingConnection::signalThread] New SignalThread");
  g_message("[SignalingConnection::signalThread] waiting ...");
  // Very crude please don't hurt me
  while (connection->runThread && (line = connection->tryRead()) != NULL) {
    string sline = line;
    g_message("[SignalingConnection::signalThread] got line %s", sline.c_str());
    connection->receive(sline);
    g_message("[SignalingConnection::signalThread] waiting ... %d", connection->runThread);
  }
  g_message("[SignalingConnection::signalThread] endthread");
  g_thread_exit(NULL);
  return nullptr;
}

static auto defaultOnNewPlayer = [] (DPID id) {};
static auto defaultOnSdp = [] (DPID id, const gchar* sdp) {};

SignalingConnection::SignalingConnection (const gchar* host, const guint port)
    :
    host(host),
    port(port),
    onNewPlayer(defaultOnNewPlayer),
    onSdp (defaultOnSdp) {
  this->client = g_socket_client_new();
  this->connection = g_socket_client_connect_to_host(this->client, host, port, NULL, NULL);
  this->sendStream = g_io_stream_get_output_stream(G_IO_STREAM(this->connection));
  this->inputStream = g_io_stream_get_input_stream(G_IO_STREAM(this->connection));
  this->receiveStream = g_data_input_stream_new(this->inputStream);

  this->start();
}

SignalingConnection::~SignalingConnection () {
  this->stop();

  g_object_unref(this->sendStream);
  g_object_unref(this->receiveStream);
  g_object_unref(this->inputStream);
  g_object_unref(this->connection);
  g_object_unref(this->client);
}

void SignalingConnection::send (gchar* message) {
  g_message("[SignalingConnection::send] Send \"%s\"", message);
  g_output_stream_write(this->sendStream, message, strlen(message), NULL, NULL);
  g_output_stream_write(this->sendStream, "\n", 1, NULL, NULL);
  g_output_stream_flush(this->sendStream, NULL, NULL);
}

gchar* SignalingConnection::tryRead () {
  gsize* size = nullptr;
  this->readCancellable = g_cancellable_new();
  auto result = g_data_input_stream_read_line(this->receiveStream, size, this->readCancellable, NULL);
  g_object_unref(this->readCancellable);
  g_message("[SignalingConnection::tryRead] Receive \"%s\"", result);
  this->readCancellable = NULL;
  return result;
}

void SignalingConnection::receive (string message) {
  g_message("[SignalingConnection::receive] %s", message.c_str());
  if (message.substr(0, 4) == "join") {
    g_message("[SignalingConnection::receive] new player str %s", message.substr(5).c_str());
    auto id = stoi(message.substr(5));
    g_message("[SignalingConnection::receive] new player %d", id);
    auto onNewPlayer = this->onNewPlayer;
    if (onNewPlayer) {
      onNewPlayer(id);
    }
  } else if (message.substr(0, 3) == "sdp") {
    auto parts = message.substr(11);
    auto sdpStart = parts.find(",sdp:");
    auto remoteId = stoi(parts.substr(0, sdpStart));
    auto remoteSdp = parts.substr(sdpStart + 5);
    auto onSdp = this->onSdp;
    if (onSdp) {
      onSdp(remoteId, remoteSdp.c_str());
    }
  } else if (message.substr(0, 5) == "enums") {
    auto id = stoi(message.substr(6));
    auto onEnumSessions = this->onEnumSessions;
    if (onEnumSessions) {
      onEnumSessions(id);
    }
  } else if (message.substr(0, 5) == "enumr") {
    auto relayed64 = message.substr(6);
    g_message("[SignalingConnection::receive] enumr : %s %d", relayed64.c_str(), relayed64.size());
    auto onEnumSessionsResponse = this->onEnumSessionsResponse;
    if (onEnumSessionsResponse) {
      gsize size = 0;
      auto relayedMessage = g_base64_decode(relayed64.c_str(), &size);
      g_message("[SignalingConnection::receive] enumr size : %d", size);
      onEnumSessionsResponse(relayedMessage, size);
    }
  }
}

void SignalingConnection::relayEnumSessions (GUID sessionGuid) {
  auto relayString = g_strdup_printf(
    "enums %s",
    to_string(sessionGuid).c_str()
  );
  this->send(relayString);
  g_free(relayString);
}

void SignalingConnection::relayEnumSessionsResponse (int messageId, void* data, gsize size) {
  auto replyString = g_strdup_printf(
    "enumr id:%d,reply:%s",
    messageId,
    g_base64_encode((const guchar*) data, size)
  );
  this->send(replyString);
  g_free(replyString);
}

void SignalingConnection::connect (GUID sessionGuid, bool isHost) {
  auto authString = g_strdup_printf(
    isHost ? "host %s" : "join %s",
    to_string(sessionGuid).c_str()
  );
  this->send(authString);
  g_free(authString);
}

void SignalingConnection::sendSdp (DPID targetId, gchar* sdp) {
  auto sdp64 = g_base64_encode((const guchar*) sdp, strlen(sdp));
  auto sdpString = g_strdup_printf("sdp to:%d,sdp:%s", static_cast<int>(targetId), sdp64);
  this->send(sdpString);
  g_free(sdpString);
  g_free(sdp64);
}

void SignalingConnection::start () {
  this->runThread = true;
  this->thread = g_thread_new("signaling client", signalThread, this);
}
void SignalingConnection::stop () {
  this->runThread = false;
  if (this->readCancellable != NULL) {
    g_cancellable_cancel(this->readCancellable);
  }
  g_thread_join(this->thread);
  this->thread = NULL;
}

}
