#pragma once
#include <winsock2.h>
#include <dplay.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <functional>
#include <string>

using std::function;
using std::string;

namespace nicesp {

class SignalingConnection {
private:
  const gchar* host;
  const guint port;

  GCancellable* readCancellable;
  GSocketClient* client;
  GSocketConnection* connection;
  GInputStream* inputStream;
  GDataInputStream* receiveStream;
  GOutputStream* sendStream;
  GThread* thread;
public:
  SignalingConnection (const gchar* host, const guint port);
  ~SignalingConnection ();

  void send (gchar* message);
  gchar* tryRead ();
  void receive (string message);

  void relayEnumSessions (GUID sessionGuid);
  void relayEnumSessionsResponse (int messageId, void* data, gsize size);
  void connect (GUID sessionGuid, DPID playerId);
  void sendSdp (DPID targetId, gchar* sdp);

  function<void(int)> onEnumSessions;
  function<void(void*, gsize)> onEnumSessionsResponse;
  function<void(DPID)> onNewPlayer;
  function<void(DPID, const gchar*)> onSdp;

  bool runThread;
  void start ();
  void stop ();
};

}
