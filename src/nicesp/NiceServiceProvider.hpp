#pragma once
#include "SignalingConnection.hpp"
#include "GameSession.hpp"
#include <dplib/DPLobby.hpp>
#include <dplib/DPServiceProvider.hpp>
#include <stack>

using std::stack;
using dplib::DPLobby;
using dplib::DPServiceProvider;

#define DX61AVERSION 11
#define DPMSG_SIGNATURE 0x79616c70
#define DPMSGCMD_ENUMSESSIONS 0x0002

namespace nicesp {

extern GMainLoop* mainLoop;

// Default empty GUID value.
static const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

struct DPSPMessageEnvelope {
  DWORD magic;
  WORD commandId;
  WORD version;

  DPSPMessageEnvelope (WORD commandId)
      :
      magic(DPMSG_SIGNATURE),
      commandId(commandId),
      version(DX61AVERSION) {
  };
};

struct DPSPEnumSessionsMessage {
  DPSPMessageEnvelope envelope;
  GUID applicationGuid;
  DWORD passwordOffset;
  DWORD flags;

  DPSPEnumSessionsMessage (GUID applicationGuid = GUID_NULL)
      :
      envelope(DPMSGCMD_ENUMSESSIONS),
      applicationGuid(applicationGuid),
      passwordOffset(0),
      flags(0) {
  };
};

class NiceServiceProvider: public DPServiceProvider {
private:
  GMainLoop* mainLoop;
  GameSession* session;
  stack<DPID> replyTargets;
  SignalingConnection* enumSessionsSC;
  // FIXME This is used to tell the signaling server which EnumSessions message we
  // are attempting to reply to. Pretty hacky, perhaps store this "reply-to
  // number" in the message header for Name Server messages instead?
  int currentEnumSessionsMessageId;

public:
  NiceServiceProvider (SPINITDATA* data);

  virtual HRESULT EnumSessions (void* message, int size, bool returnStatus);
  virtual HRESULT Open (DPSP_OPENDATA* data);
  virtual HRESULT CreatePlayer (DPSP_CREATEPLAYERDATA* data);
  virtual HRESULT Send (DPSP_SENDDATA* data);
  virtual HRESULT Reply (void* spHeader, void* message, int size, DPID nameServer);
  virtual HRESULT Close ();
  virtual HRESULT Shutdown ();
};

}