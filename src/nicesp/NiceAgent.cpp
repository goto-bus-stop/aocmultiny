#include "NiceAgent.hpp"

namespace nicesp {

NiceCredentials::~NiceCredentials () {
  g_free(this->ufrag);
  g_free(this->pwd);
}

NiceAgent::NiceAgent (GMainContext* ctx, NiceCompatibility compat)
    :
    na(nice_agent_new(ctx, compat)) {
}

NiceAgent::NiceAgent (InternalAgent na)
    :
    na(na) {
  g_object_ref(this->na);
}

NiceAgent::~NiceAgent () {
  g_object_unref(this->na);
}

InternalAgent NiceAgent::unwrap () {
  g_debug("[NiceAgent::unwrap]");
  return this->na;
}

gboolean NiceAgent::addLocalAddress (NiceAddress* address) {
  g_debug("[NiceAgent::nice_agent_add_local_address]");
  return nice_agent_add_local_address(this->na, address);
}

NiceStream* NiceAgent::getStream (guint streamId) {
  g_debug("[NiceAgent::getStream]");
  return new NiceStream(this, streamId);
}

NiceStream* NiceAgent::addStream (guint components) {
  g_debug("[NiceAgent::nice_agent_add_stream]");
  auto id = nice_agent_add_stream(this->na, components);
  return this->getStream(id);
}

void NiceAgent::removeStream (guint streamId) {
  g_debug("[NiceAgent::nice_agent_remove_stream]");
  return nice_agent_remove_stream(this->na, streamId);
}

void NiceAgent::setPortRange (guint streamId, guint componentId, guint minPort, guint maxPort) {
  g_debug("[NiceAgent::nice_agent_set_port_range]");
  nice_agent_set_port_range(this->na, streamId, componentId, minPort, maxPort);
}

gboolean NiceAgent::setRelayInfo (
  guint streamId,
  guint componentId,
  const gchar* ip,
  guint port,
  const gchar* username,
  const gchar* password,
  NiceRelayType type
) {
  g_debug("[NiceAgent::nice_agent_set_relay_info]");
  return nice_agent_set_relay_info(this->na, streamId, componentId, ip, port, username, password, type);
}

gboolean NiceAgent::gatherCandidates (guint streamId) {
  g_debug("[NiceAgent::nice_agent_gather_candidates]");
  return nice_agent_gather_candidates(this->na, streamId);
}
gboolean NiceAgent::setRemoteCredentials (guint streamId, const gchar* ufrag, const gchar* pwd) {

  g_debug("[NiceAgent::nice_agent_set_remote_credentials]");
  return nice_agent_set_remote_credentials(this->na, streamId, ufrag, pwd);
}

gboolean NiceAgent::setLocalCredentials (guint streamId, const gchar* ufrag, const gchar* pwd) {
  g_debug("[NiceAgent::nice_agent_set_local_credentials]");
  return nice_agent_set_local_credentials(this->na, streamId, ufrag, pwd);
}

NiceCredentials* NiceAgent::getLocalCredentials (guint streamId) {
  g_debug("[NiceAgent::nice_agent_get_local_credentials]");
  auto creds = new NiceCredentials;
  if (nice_agent_get_local_credentials(this->na, streamId, &creds->ufrag, &creds->pwd)) {
    return creds;
  }
  return nullptr;
}

int NiceAgent::setRemoteCandidates (guint streamId, guint componentId, const GSList* candidates) {
  g_debug("[NiceAgent::nice_agent_set_remote_candidates]");
  return nice_agent_set_remote_candidates(this->na, streamId, componentId, candidates);
}

gint NiceAgent::send (guint streamId, guint componentId, guint len, const gchar *buf) {
  g_debug("[NiceAgent::nice_agent_send]");
  return nice_agent_send(this->na, streamId, componentId, len, buf);
}

gint NiceAgent::sendNonBlocking (
  guint streamId,
  guint componentId,
  const NiceOutputMessage* messages,
  guint messagesCount,
  GCancellable* cancellable,
  GError** error
) {
  g_debug("[NiceAgent::nice_agent_send_messages_nonblocking]");
  return nice_agent_send_messages_nonblocking(this->na, streamId, componentId, messages, messagesCount, cancellable, error);
}

GSList* NiceAgent::getLocalCandidates (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_local_candidates]");
  return nice_agent_get_local_candidates(this->na, streamId, componentId);
}
GSList* NiceAgent::getRemoteCandidates (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_remote_candidates]");
  return nice_agent_get_remote_candidates(this->na, streamId, componentId);
}

gboolean NiceAgent::restart () {
  g_debug("[NiceAgent::nice_agent_restart]");
  return nice_agent_restart(this->na);
}
gboolean NiceAgent::restartStream (guint streamId) {
  g_debug("[NiceAgent::nice_agent_restart_stream]");
  return nice_agent_restart_stream(this->na, streamId);
}

gboolean NiceAgent::attachRecv (guint streamId, guint componentId, GMainContext* ctx, NiceAgentRecvFunc func, gpointer data) {
  g_debug("[NiceAgent::nice_agent_attach_recv]");
  return nice_agent_attach_recv(this->na, streamId, componentId, ctx, func, data);
}
gssize NiceAgent::recv (guint streamId, guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error) {
  g_debug("[NiceAgent::nice_agent_recv]");
  return nice_agent_recv(this->na, streamId, componentId, buf, len, cancellable, error);
}

gint NiceAgent::recvMessages (guint streamId, guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error) {
  g_debug("[NiceAgent::nice_agent_recv_messages]");
  return nice_agent_recv_messages(this->na, streamId, componentId, messages, messagesCount, cancellable, error);
}
gssize NiceAgent::recvNonBlocking (guint streamId, guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error) {
  g_debug("[NiceAgent::nice_agent_recv_nonblocking]");
  return nice_agent_recv_nonblocking(this->na, streamId, componentId, buf, len, cancellable, error);
}
gssize NiceAgent::recvMessagesNonBlocking (guint streamId, guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error) {
  g_debug("[NiceAgent::nice_agent_recv_messages_nonblocking]");
  return nice_agent_recv_messages_nonblocking(this->na, streamId, componentId, messages, messagesCount, cancellable, error);
}

gboolean NiceAgent::setSelectedPair (guint streamId, guint componentId, const gchar* lfoundation, const gchar* rfoundation) {
  g_debug("[NiceAgent::nice_agent_set_selected_pair]");
  return nice_agent_set_selected_pair(this->na, streamId, componentId, lfoundation, rfoundation);
}
gboolean NiceAgent::getSelectedPair (guint streamId, guint componentId, NiceCandidate** local, NiceCandidate** remote) {
  g_debug("[NiceAgent::nice_agent_get_selected_pair]");
  return nice_agent_get_selected_pair(this->na, streamId, componentId, local, remote);
}
GSocket* NiceAgent::getSelectedSocket (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_selected_socket]");
  return nice_agent_get_selected_socket(this->na, streamId, componentId);
}
gboolean NiceAgent::setSelectedRemoteCandidate (guint streamId, guint componentId, NiceCandidate* candidate) {
  g_debug("[NiceAgent::nice_agent_set_selected_remote_candidate]");
  return nice_agent_set_selected_remote_candidate(this->na, streamId, componentId, candidate);
}
void NiceAgent::setStreamToS (guint streamId, gint tos) {
  nice_agent_set_stream_tos(this->na, streamId, tos);
}
void NiceAgent::setSoftware (const gchar* software) {
  nice_agent_set_software(this->na, software);
}
gboolean NiceAgent::setStreamName (guint streamId, const gchar* name) {
  g_debug("[NiceAgent::nice_agent_set_stream_name]");
  return nice_agent_set_stream_name(this->na, streamId, name);
}
const gchar* NiceAgent::getStreamName (guint streamId) {
  g_debug("[NiceAgent::nice_agent_get_stream_name]");
  return nice_agent_get_stream_name(this->na, streamId);
}
NiceCandidate* NiceAgent::getDefaultLocalCandidate (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_default_local_candidate]");
  return nice_agent_get_default_local_candidate(this->na, streamId, componentId);
}
gchar* NiceAgent::generateLocalSdp () {
  g_debug("[NiceAgent::nice_agent_generate_local_sdp]");
  return nice_agent_generate_local_sdp(this->na);
}
gchar* NiceAgent::generateLocalStreamSdp (guint streamId, gboolean includeNonIce) {
  g_debug("[NiceAgent::nice_agent_generate_local_stream_sdp]");
  return nice_agent_generate_local_stream_sdp(this->na, streamId, includeNonIce);
}
gchar* NiceAgent::generateLocalCandidateSdp (NiceCandidate* candidate) {
  g_debug("[NiceAgent::nice_agent_generate_local_candidate_sdp]");
  return nice_agent_generate_local_candidate_sdp(this->na, candidate);
}
int NiceAgent::parseRemoteSdp (const gchar* sdp) {
  g_debug("[NiceAgent::nice_agent_parse_remote_sdp]");
  return nice_agent_parse_remote_sdp(this->na, sdp);
}
GSList* NiceAgent::parseRemoteStreamSdp (guint streamId, const gchar* sdp, gchar** ufrag, gchar** pwd) {
  g_debug("[NiceAgent::nice_agent_parse_remote_stream_sdp]");
  return nice_agent_parse_remote_stream_sdp(this->na, streamId, sdp, ufrag, pwd);
}
NiceCandidate* NiceAgent::parseRemoteCandidateSdp (guint streamId, const gchar* sdp) {
  g_debug("[NiceAgent::nice_agent_parse_remote_candidate_sdp]");
  return nice_agent_parse_remote_candidate_sdp(this->na, streamId, sdp);
}
GIOStream* NiceAgent::getIOStream (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_io_stream]");
  return nice_agent_get_io_stream(this->na, streamId, componentId);
}

gboolean NiceAgent::forgetRelays (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_forget_relays]");
  return nice_agent_forget_relays(this->na, streamId, componentId);
}
NiceComponentState NiceAgent::getComponentState (guint streamId, guint componentId) {
  g_debug("[NiceAgent::nice_agent_get_component_state]");
  return nice_agent_get_component_state(this->na, streamId, componentId);
}


NiceStream::NiceStream (NiceAgent* agent, guint id)
    :
    agent(agent),
    id(id) {
}

void NiceStream::remove () {
  this->agent->removeStream(this->id);
}

void NiceStream::setPortRange (guint componentId, guint minPort, guint maxPort) {
  this->agent->setPortRange(this->id, componentId, minPort, maxPort);
};

gboolean NiceStream::setRelayInfo (
  guint componentId,
  const gchar* ip,
  guint port,
  const gchar* username,
  const gchar* password,
  NiceRelayType type
) {
  g_debug("[NiceAgent::setRelayInfo]");
  return this->agent->setRelayInfo(this->id, componentId, ip, port, username, password, type);
}

gboolean NiceStream::gatherCandidates () {
  g_debug("[NiceAgent::gatherCandidates]");
  return this->agent->gatherCandidates(this->id);
};

gboolean NiceStream::setRemoteCredentials (const gchar* ufrag, const gchar* pwd) {
  g_debug("[NiceStream::setRemoteCredentials]");
  return this->agent->setRemoteCredentials(this->id, ufrag, pwd);
}

gboolean NiceStream::setLocalCredentials (const gchar* ufrag, const gchar* pwd) {
  g_debug("[NiceStream::setLocalCredentials]");
  return this->agent->setLocalCredentials(this->id, ufrag, pwd);
}

NiceCredentials* NiceStream::getLocalCredentials () {
  g_debug("[NiceStream::getLocalCredentials]");
  return this->agent->getLocalCredentials(this->id);
}

int NiceStream::setRemoteCandidates (guint componentId, const GSList* candidates) {
  g_debug("[NiceStream::setRemoteCandidates]");
  return this->agent->setRemoteCandidates(this->id, componentId, candidates);
}

gint NiceStream::send (guint componentId, guint len, const gchar *buf) {
  g_debug("[NiceStream::send]");
  return this->agent->send(this->id, componentId, len, buf);
}

gint NiceStream::sendNonBlocking (
  guint componentId,
  const NiceOutputMessage* messages,
  guint messagesCount,
  GCancellable* cancellable,
  GError** error
) {
  g_debug("[NiceStream::sendNonBlocking]");
  return this->agent->sendNonBlocking(this->id, componentId, messages, messagesCount, cancellable, error);
}

GSList* NiceStream::getLocalCandidates (guint componentId) {
  g_debug("[NiceStream::getLocalCandidates]");
  return this->agent->getLocalCandidates(this->id, componentId);
}

GSList* NiceStream::getRemoteCandidates (guint componentId) {
  g_debug("[NiceStream::getRemoteCandidates]");
  return this->agent->getRemoteCandidates(this->id, componentId);
}

gboolean NiceStream::restart () {
  g_debug("[NiceStream::restartStream]");
  return this->agent->restartStream(this->id);
}

gboolean NiceStream::attachRecv (guint componentId, GMainContext* ctx, NiceAgentRecvFunc func, gpointer data) {
  g_debug("[NiceStream::attachRecv]");
  return this->agent->attachRecv(this->id, componentId, ctx, func, data);
}

gssize NiceStream::recv (guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error) {
  g_debug("[NiceStream::recv]");
  return this->agent->recv(this->id, componentId, buf, len, cancellable, error);
}
gint NiceStream::recvMessages (guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error) {
  g_debug("[NiceStream::recvMessages]");
  return this->agent->recvMessages(this->id, componentId, messages, messagesCount, cancellable, error);
}
gssize NiceStream::recvNonBlocking (guint componentId, guint8* buf, gsize len, GCancellable* cancellable, GError** error) {
  g_debug("[NiceStream::recvNonBlocking]");
  return this->agent->recvNonBlocking(this->id, componentId, buf, len, cancellable, error);
}
gssize NiceStream::recvMessagesNonBlocking (guint componentId, NiceInputMessage* messages, guint messagesCount, GCancellable* cancellable, GError** error) {
  g_debug("[NiceStream::recvMessagesNonBlocking]");
  return this->agent->recvMessagesNonBlocking(this->id, componentId, messages, messagesCount, cancellable, error);
}

gboolean NiceStream::setSelectedPair (guint componentId, const gchar* lfoundation, const gchar* rfoundation) {
  g_debug("[NiceStream::setSelectedPair]");
  return this->agent->setSelectedPair(this->id, componentId, lfoundation, rfoundation);
}
gboolean NiceStream::getSelectedPair (guint componentId, NiceCandidate** local, NiceCandidate** remote) {
  g_debug("[NiceStream::getSelectedPair]");
  return this->agent->getSelectedPair(this->id, componentId, local, remote);
}

GSocket* NiceStream::getSelectedSocket (guint componentId) {
  g_debug("[NiceStream::getSelectedSocket]");
  return this->agent->getSelectedSocket(this->id, componentId);
}

gboolean NiceStream::setSelectedRemoteCandidate (guint componentId, NiceCandidate* candidate) {
  g_debug("[NiceStream::setSelectedRemoteCandidate]");
  return this->agent->setSelectedRemoteCandidate(this->id, componentId, candidate);
}

void NiceStream::setToS (gint tos) {
  g_debug("[NiceStream::setStreamToS]");
  return this->agent->setStreamToS(this->id, tos);
}

gboolean NiceStream::setName (const gchar* name) {
  g_debug("[NiceStream::setStreamName]");
  return this->agent->setStreamName(this->id, name);
}

const gchar* NiceStream::getName () {
  g_debug("[NiceStream::getStreamName]");
  return this->agent->getStreamName(this->id);
}

NiceCandidate* NiceStream::getDefaultLocalCandidate (guint componentId) {
  g_debug("[NiceStream::getDefaultLocalCandidate]");
  return this->agent->getDefaultLocalCandidate(this->id, componentId);
}

gchar* NiceStream::generateLocalStreamSdp (gboolean includeNonIce) {
  g_debug("[NiceStream::generateLocalStreamSdp]");
  return this->agent->generateLocalStreamSdp(this->id, includeNonIce);
}

GSList* NiceStream::parseRemoteStreamSdp (const gchar* sdp, gchar** ufrag, gchar** pwd) {
  g_debug("[NiceStream::parseRemoteStreamSdp]");
  return this->agent->parseRemoteStreamSdp(this->id, sdp, ufrag, pwd);
}

NiceCandidate* NiceStream::parseRemoteCandidateSdp (const gchar* sdp) {
  g_debug("[NiceStream::parseRemoteCandidateSdp]");
  return this->agent->parseRemoteCandidateSdp(this->id, sdp);
}

GIOStream* NiceStream::getIOStream (guint componentId) {
  g_debug("[NiceStream::getIOStream]");
  return this->agent->getIOStream(this->id, componentId);
}

gboolean NiceStream::forgetRelays (guint componentId) {
  g_debug("[NiceStream::forgetRelays]");
  return this->agent->forgetRelays(this->id, componentId);
}

NiceComponentState NiceStream::getComponentState (guint componentId) {
  g_debug("[NiceStream::getComponentState]");
  return this->agent->getComponentState(this->id, componentId);
}

}
