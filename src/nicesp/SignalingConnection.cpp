#include "SignalingConnection.hpp"
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

using std::string;

namespace nicesp {

static char* gtc (GUID guid) {
  auto str = new wchar_t[51];
  StringFromGUID2(guid, str, 50);
  auto result = new char[wcslen(str)];
  for (uint32_t i = 0; i < 38; i++) {
    result[i] = static_cast<char>(str[i]);
  }
  result[38] = '\0';
  return result;
}

static void* signalThread (void* data) {
  auto connection = reinterpret_cast<SignalingConnection*>(data);
  gchar* line = nullptr;
  g_message("New SignalThread");
  g_message("waiting ...");
  // Very crude please don't hurt me
  while (connection->runThread && (line = connection->tryRead()) != NULL) {
    string sline = line;
    g_message("got line %s", sline.c_str());
    connection->receive(sline);
    g_message("waiting ... %d", connection->runThread);
  }
  g_message("endthread");
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
  g_message("Send \"%s\"", message);
  g_output_stream_write(this->sendStream, message, strlen(message), NULL, NULL);
  g_output_stream_write(this->sendStream, "\n", 1, NULL, NULL);
  g_output_stream_flush(this->sendStream, NULL, NULL);
}

gchar* SignalingConnection::tryRead () {
  gsize* size = nullptr;
  this->readCancellable = g_cancellable_new();
  auto result = g_data_input_stream_read_line(this->receiveStream, size, this->readCancellable, NULL);
  g_object_unref(this->readCancellable);
  g_message("Receive \"%s\"", result);
  this->readCancellable = NULL;
  return result;
}

void SignalingConnection::receive (string message) {
  if (message.substr(0, 6) == "player") {
    auto idStr = stoi(message.substr(10));
    g_message("new player %d", idStr);
    auto onNewPlayer = this->onNewPlayer;
    if (onNewPlayer) {
      onNewPlayer(idStr);
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
  }
}

void SignalingConnection::connect (GUID sessionGuid, DPID playerId) {
  auto authString = g_strdup_printf("create session:%s,id:%d", gtc(sessionGuid), static_cast<int>(playerId));
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
