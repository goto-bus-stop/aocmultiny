#include "GameSession.hpp"
#include "nicesp.hpp"
#include "../util.hpp"
#include <gio/gnetworking.h>
#include <string>

using namespace std;

namespace nicesp {

const gchar* DEFAULT_SIGNALING_HOST = "localhost";
const guint DEFAULT_SIGNALING_PORT = 7788;

GMainLoop* gloop;
GameSession* _session;
IDirectPlayLobby3A* _lobby;

static GameSession* getGameSession (IDirectPlaySP* provider) {
  return _session;
#if 0
  void* session;
  DWORD size = sizeof(session);
  provider->GetSPData(&session, &size, DPGET_LOCAL);
  return static_cast<GameSession*>(session);
#endif
}

/**
 * Get the Lobby connection settings for this session.
 * (Are Service Providers suppposed to access this? Well, it works, so…)
 */
DPLCONNECTION* getDPLConnection () {
  DWORD size;
  _lobby->GetConnectionSettings(0, NULL, &size);
  auto conndata = malloc(size);
  auto hr = _lobby->GetConnectionSettings(0, conndata, &size);
  if (FAILED(hr)) {
    return NULL;
  }
  return static_cast<DPLCONNECTION*>(conndata);
}

GUID getSessionGuid () {
  auto connection = getDPLConnection();
  auto guid = connection->lpSessionDesc->guidInstance;
  free(connection);
  return guid;
}

static SignalingConnection* enumSessionsSC;

static HRESULT WINAPI DPNice_EnumSessions (DPSP_ENUMSESSIONSDATA* data) {
  g_message(
    "EnumSessions (%p,%ld,%p,%u) stub",
    data->lpMessage, data->dwMessageSize,
    data->lpISP, data->bReturnStatus
  );

  auto provider = data->lpISP;
  auto guid = getSessionGuid();
  g_message("[EnumSessions] guidInstance = %s", to_string(guid).c_str());

  if (!enumSessionsSC) {
    enumSessionsSC = new SignalingConnection(
      DEFAULT_SIGNALING_HOST, DEFAULT_SIGNALING_PORT);
    enumSessionsSC->onEnumSessionsResponse = [provider] (auto data, auto size) {
      g_message("[EnumSessions] Discovered session %d", size);
      provider->HandleMessage(data, size, NULL);
    };
  }
  enumSessionsSC->relayEnumSessions(getSessionGuid());

  return DP_OK;
}

// FIXME This is used to tell the signaling server which EnumSessions message we
// are attempting to reply to. Pretty hacky, perhaps store this "reply-to
// number" in the message header for Name Server messages instead?
static int currentEnumSessionsMessageId = 0;

static HRESULT WINAPI DPNice_Reply (DPSP_REPLYDATA* data) {
  g_message(
    "Reply (%p,%p,%ld,%ld,%p) stub",
    data->lpSPMessageHeader, data->lpMessage, data->dwMessageSize,
    data->idNameServer, data->lpISP
  );

  auto session = getGameSession(data->lpISP);
  // LOL. Careful! Assuming here that this is indeed an enumsessions reply message.
  // FIXME Really actually only reply to enumsessions messages here. Perhaps by
  // parsing the message envelope to check.
  if (currentEnumSessionsMessageId != 0) {
    session->getSignalingConnection()->relayEnumSessionsResponse(
      currentEnumSessionsMessageId,
      data->lpMessage,
      data->dwMessageSize
    );
    currentEnumSessionsMessageId = 0;
  }

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Send (DPSP_SENDDATA* data) {
  g_message(
    "Send (0x%08lx,%ld,%ld,%p,%ld,%u,%p)",
    data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpMessage, data->dwMessageSize,
    data->bSystemMessage, data->lpISP
  );

  auto session = getGameSession(data->lpISP);

  auto player = session->getPlayerById(data->idPlayerTo);
  auto stream = player->agent->getStream(1);

  stream->send(1, data->dwMessageSize, static_cast<gchar*>(data->lpMessage));

  return DP_OK;
}

static HRESULT WINAPI DPNice_CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  g_message(
    "CreatePlayer (%ld,0x%08lx,%p,%p)",
    data->idPlayer, data->dwFlags,
    data->lpSPMessageHeader, data->lpISP
  );

  auto session = getGameSession(data->lpISP);

  if (data->dwFlags & (DPLAYI_PLAYER_PLAYERLOCAL | DPLAYI_PLAYER_NAMESRVR)) {
    // Creating our local player, register with the signaling server.
    session->getSignalingConnection()
      ->connect(getSessionGuid(), data->idPlayer);
  }

  return DP_OK;
}

static HRESULT WINAPI DPNice_DeletePlayer (DPSP_DELETEPLAYERDATA* data) {
  g_message(
    "DeletePlayer (%ld,0x%08lx,%p)",
    data->idPlayer, data->dwFlags, data->lpISP
  );

  auto session = getGameSession(data->lpISP);

  session->deletePlayer(data->idPlayer);

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddress (DPSP_GETADDRESSDATA* data) {
  g_message(
    "GetAddress (%ld,0x%08lx,%p,%p,%p) stub",
    data->idPlayer, data->dwFlags, data->lpAddress,
    data->lpdwAddressSize, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetCaps (DPSP_GETCAPSDATA* data) {
  g_message(
    "GetCaps (%ld,%p,0x%08lx,%p)",
    data->idPlayer, data->lpCaps, data->dwFlags, data->lpISP
  );

  data->lpCaps->dwFlags = DPCAPS_ASYNCSUPPORTED;
  data->lpCaps->dwMaxBufferSize = 1024;
  data->lpCaps->dwMaxQueueSize = 0;
  data->lpCaps->dwMaxPlayers = 65536;
  data->lpCaps->dwHundredBaud = 0;
  data->lpCaps->dwLatency = 50;
  data->lpCaps->dwMaxLocalPlayers = 65536;
  data->lpCaps->dwHeaderLength = 0;
  data->lpCaps->dwTimeout = 500;

  return DP_OK;
}

static void* startThread (void*) {
  CoInitialize(NULL);

  g_message("loop start");
  g_main_loop_run(gloop);
  g_message("loop complete");
  g_main_loop_unref(gloop);

  CoUninitialize();
  g_thread_exit(NULL);
  return nullptr;
}

// TODO include the header file that has this stuff and use that instead.
struct DPSP_MSG_ENUMSESSIONS {
  struct {
    DWORD magic;
    WORD commandId;
    WORD version;
  } envelope;
  GUID applicationGuid;
  DWORD passwordOffset;
  DWORD flags;
};

static HRESULT WINAPI DPNice_Open (DPSP_OPENDATA* data) {
  g_message(
    "Open (%u,%p,%p,%u,0x%08lx,0x%08lx)",
    data->bCreate, data->lpSPMessageHeader, data->lpISP,
    data->bReturnStatus, data->dwOpenFlags, data->dwSessionFlags
  );

  g_thread_new("main loop", &startThread, NULL);

  if (enumSessionsSC) {
    delete enumSessionsSC;
    enumSessionsSC = NULL;
  }

  g_message("new session");
  auto host = DEFAULT_SIGNALING_HOST;
  auto port = DEFAULT_SIGNALING_PORT;
  // TODO store in DirectPlay SPData (the pointers got weird when I tried this)
  auto connection = new SignalingConnection(host, port);
  auto session = new GameSession(connection, !!data->bCreate);

  auto sessionRef = &session;
  auto provider = data->lpISP;
  provider->SetSPData(&sessionRef, sizeof(sessionRef), DPSET_LOCAL);

  connection->onEnumSessions = [provider] (auto id) {
    g_message("onEnumSessions %d", id);
    // The signaling server asked us to send info about local sessions, so
    // create a fake ENUMSESSIONS message for DirectPlay
    auto message = new DPSP_MSG_ENUMSESSIONS;
    message->envelope.magic = 0x79616c70;
    message->envelope.commandId = 0x02;
    message->envelope.version = 0x0b;
    message->flags = 0;
    message->passwordOffset = 0;
    message->applicationGuid = getDPLConnection()->lpSessionDesc->guidApplication;
    currentEnumSessionsMessageId = id;
    provider->HandleMessage(message, sizeof(*message), NULL);
  };

  // FIXME Hack. Used to work around about SPData stuff^
  _session = session;

  return DP_OK;
}

static HRESULT WINAPI DPNice_CloseEx (DPSP_CLOSEDATA* data) {
  g_message("CloseEx (%p) stub", data->lpISP);

  auto session = getGameSession(data->lpISP);

  g_main_loop_quit(gloop);
  delete session;

  return DP_OK;
}

static HRESULT WINAPI DPNice_ShutdownEx (DPSP_SHUTDOWNDATA* data) {
  g_message("ShutdownEx (%p) stub", data->lpISP);
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddressChoices (DPSP_GETADDRESSCHOICESDATA* data) {
  g_message(
    "GetAddressChoices (%p,%p,%p) stub",
    data->lpAddress, data->lpdwAddressSize, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendEx (DPSP_SENDEXDATA* data) {
  g_message(
    "SendEx (%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p,%u) stub",
    data->lpISP, data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID, data->bSystemMessage
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendToGroupEx (DPSP_SENDTOGROUPEXDATA* data) {
  g_message(
    "SendToGroupEx (%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p) stub",
    data->lpISP, data->dwFlags, data->idGroupTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Cancel (DPSP_CANCELDATA* data) {
  g_message(
    "Cancel (%p,0x%08lx,%p,%ld,0x%08lx,0x%08lx) stub",
    data->lpISP, data->dwFlags, data->lprglpvSPMsgID, data->cSPMsgID,
    data->dwMinPriority, data->dwMaxPriority
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetMessageQueue (DPSP_GETMESSAGEQUEUEDATA* data) {
  g_message(
    "GetMessageQueue (%p,0x%08lx,%ld,%ld,%p,%p) stub",
    data->lpISP, data->dwFlags, data->idFrom, data->idTo,
    data->lpdwNumMsgs, data->lpdwNumBytes
  );
  return DPERR_UNSUPPORTED;
}

static void setup_callbacks (DPSP_SPCALLBACKS* callbacks) {
  callbacks->EnumSessions = DPNice_EnumSessions;
  callbacks->Reply = DPNice_Reply;
  callbacks->Send = DPNice_Send;
  callbacks->CreatePlayer = DPNice_CreatePlayer;
  callbacks->DeletePlayer = DPNice_DeletePlayer;
  callbacks->GetAddress = DPNice_GetAddress;
  callbacks->GetCaps = DPNice_GetCaps;
  callbacks->Open = DPNice_Open;
  callbacks->CloseEx = DPNice_CloseEx;
  callbacks->ShutdownEx = DPNice_ShutdownEx;
  callbacks->GetAddressChoices = DPNice_GetAddressChoices;
  callbacks->SendEx = DPNice_SendEx;
  callbacks->SendToGroupEx = DPNice_SendToGroupEx;
  callbacks->Cancel = DPNice_Cancel;
  callbacks->GetMessageQueue = DPNice_GetMessageQueue;

  callbacks->AddPlayerToGroup = NULL;
  callbacks->Close = NULL;
  callbacks->CreateGroup = NULL;
  callbacks->DeleteGroup = NULL;
  callbacks->RemovePlayerFromGroup = NULL;
  callbacks->SendToGroup = NULL;
  callbacks->Shutdown = NULL;
}

static HRESULT init (SPINITDATA* spData) {
  g_message("SPInit");

  g_message("Initializing library for %ls (%ls)", to_string(*spData->lpGuid).c_str(), spData->lpszName);

  /* We only support NICE service */
  if (!IsEqualGUID(*spData->lpGuid, DPSPGUID_NICE)) {
    return DPERR_UNAVAILABLE;
  }

  /* Assign callback functions */
  setup_callbacks(spData->lpCB);

  g_message("Have callbacks");

  gloop = g_main_loop_new(NULL, FALSE);

  /* dplay needs to know the size of the header */
  spData->dwSPHeaderSize = 0;
  spData->dwSPVersion = DPSP_MAJORVERSIONMASK & DPSP_MAJORVERSION;

  return DP_OK;
}

}

__declspec(dllexport) HRESULT __cdecl SPInit (SPINITDATA* spData) {
  return nicesp::init(spData);
}

BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, void* reserved) {
  g_message("DllMain (0x%p, %ld, %p)", instance, reason, reserved);

  switch (reason) {
  case DLL_PROCESS_ATTACH:
    CoCreateInstance(
      CLSID_DirectPlayLobby,
      NULL,
      CLSCTX_ALL,
      IID_IDirectPlayLobby3A,
      reinterpret_cast<void**>(&nicesp::_lobby)
    );

    DisableThreadLibraryCalls(instance);
    break;
  case DLL_PROCESS_DETACH:
    nicesp::_lobby->Release();
    break;
  default:
    break;
  }
  return TRUE;
}
