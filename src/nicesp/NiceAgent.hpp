#pragma once
#include <agent.h>

namespace nicesp {

typedef ::NiceAgent* InternalAgent;

class NiceAgent;
class NiceStream;

typedef struct NiceCredentials {
  gchar* ufrag;
  gchar* pwd;

  ~NiceCredentials ();
} NiceCredentials;

class NiceAgent {
private:
  InternalAgent na;
public:
  NiceAgent (GMainContext* ctx, NiceCompatibility compat = NICE_COMPATIBILITY_RFC5245);
  NiceAgent (InternalAgent na);
  ~NiceAgent ();

  InternalAgent unwrap ();
  gboolean addLocalAddress (NiceAddress* address);
  NiceStream* getStream (guint streamId);
  NiceStream* addStream (guint components);
  void removeStream (guint streamId);
  void setPortRange (guint streamId, guint componentId, guint minPort, guint maxPort);
  gboolean setRelayInfo (
    guint streamId,
    guint componentId,
    const gchar* ip,
    guint port,
    const gchar* username,
    const gchar* password,
    NiceRelayType type
  );
  gboolean gatherCandidates (guint streamId);
  gboolean setRemoteCredentials (guint streamId, const gchar* ufrag, const gchar* pwd);
  gboolean setLocalCredentials (guint streamId, const gchar* ufrag, const gchar* pwd);
  NiceCredentials* getLocalCredentials (guint streamId);
  int setRemoteCandidates (guint streamId, guint componentId, const GSList* candidates);
  gint send (guint streamId, guint componentId, guint len, const gchar *buf);
  gint sendNonBlocking (
    guint streamId,
    guint componentId,
    const NiceOutputMessage* messages,
    guint messagesCount,
    GCancellable* cancellable,
    GError** error
  );
  GSList* getLocalCandidates (guint streamId, guint componentId);
  GSList* getRemoteCandidates (guint streamId, guint componentId);
  gboolean restart ();
  gboolean restartStream (guint streamId);
  gboolean attachRecv (guint streamId, guint componentId, GMainContext* ctx, NiceAgentRecvFunc func, gpointer data);
  gssize recv (guint streamId, guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error);
  gint recvMessages (guint streamId, guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error);
  gssize recvNonBlocking (guint streamId, guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error);
  gssize recvMessagesNonBlocking (guint streamId, guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error);
  gboolean setSelectedPair (guint streamId, guint componentId, const gchar* lfoundation, const gchar* rfoundation);
  gboolean getSelectedPair (guint streamId, guint componentId, NiceCandidate** local, NiceCandidate** remote);
  GSocket* getSelectedSocket (guint streamId, guint componentId);
  gboolean setSelectedRemoteCandidate (guint streamId, guint componentId, NiceCandidate* candidate);
  void setStreamToS (guint streamId, gint tos);
  void setSoftware (const gchar* software);
  gboolean setStreamName (guint streamId, const gchar* name);
  const gchar* getStreamName (guint streamId);
  NiceCandidate* getDefaultLocalCandidate (guint streamId, guint componentId);
  gchar* generateLocalSdp ();
  gchar* generateLocalStreamSdp (guint streamId, gboolean includeNonIce);
  gchar* generateLocalCandidateSdp (NiceCandidate* candidate);
  int parseRemoteSdp (const gchar* sdp);
  GSList* parseRemoteStreamSdp (guint streamId, const gchar* sdp, gchar** ufrag, gchar** pwd);
  NiceCandidate* parseRemoteCandidateSdp (guint streamId, const gchar* sdp);
  GIOStream* getIOStream (guint streamId, guint componentId);
  gboolean forgetRelays (guint streamId, guint componentId);
  NiceComponentState getComponentState (guint streamId, guint componentId);
};

class NiceStream {
protected:
  NiceAgent* agent;
public:
  guint id;

  NiceStream (NiceAgent* agent, guint id);

  void remove ();

  void setPortRange (guint componentId, guint minPort, guint maxPort);
  gboolean setRelayInfo (
    guint componentId,
    const gchar* ip,
    guint port,
    const gchar* username,
    const gchar* password,
    NiceRelayType type
  );
  gboolean gatherCandidates ();
  gboolean setRemoteCredentials (const gchar* ufrag, const gchar* pwd);
  gboolean setLocalCredentials (const gchar* ufrag, const gchar* pwd);
  NiceCredentials* getLocalCredentials ();
  int setRemoteCandidates (guint componentId, const GSList* candidates);
  gint send (guint componentId, guint len, const gchar *buf);
  gint sendNonBlocking (
    guint componentId,
    const NiceOutputMessage* messages,
    guint messagesCount,
    GCancellable* cancellable,
    GError** error
  );
  GSList* getLocalCandidates (guint componentId);
  GSList* getRemoteCandidates (guint componentId);
  gboolean restart ();
  gboolean attachRecv (guint componentId, GMainContext* ctx, NiceAgentRecvFunc func, gpointer data);
  gssize recv (guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error);
  gint recvMessages (guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error);
  gssize recvNonBlocking (guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error);
  gssize recvMessagesNonBlocking (guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error);
  gboolean setSelectedPair (guint componentId, const gchar* lfoundation, const gchar* rfoundation);
  gboolean getSelectedPair (guint componentId, NiceCandidate** local, NiceCandidate** remote);
  GSocket* getSelectedSocket (guint componentId);
  gboolean setSelectedRemoteCandidate (guint componentId, NiceCandidate* candidate);
  void setToS (gint tos);
  gboolean setName (const gchar* name);
  const gchar* getName ();
  NiceCandidate* getDefaultLocalCandidate (guint componentId);
  gchar* generateLocalStreamSdp (gboolean includeNonIce);
  GSList* parseRemoteStreamSdp (const gchar* sdp, gchar** ufrag, gchar** pwd);
  NiceCandidate* parseRemoteCandidateSdp (const gchar* sdp);
  GIOStream* getIOStream (guint componentId);
  gboolean forgetRelays (guint componentId);
  NiceComponentState getComponentState (guint componentId);
};

}
