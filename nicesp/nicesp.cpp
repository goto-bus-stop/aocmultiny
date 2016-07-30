#include "GameSession.hpp"
#include "nicesp.hpp"
#include <gio/gnetworking.h>
#include <string>

using namespace std;

#define DEFAULT_SIGNALING_HOST "localhost"
#define DEFAULT_SIGNALING_PORT 7788

namespace nicesp {

GMainLoop* gloop;
GameSession* session;

static HRESULT WINAPI DPNice_EnumSessions (DPSP_ENUMSESSIONSDATA* data) {
  g_message(
    "EnumSessions (%p,%ld,%p,%u) stub",
    data->lpMessage, data->dwMessageSize,
    data->lpISP, data->bReturnStatus
  );

  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Reply (DPSP_REPLYDATA* data) {
  g_message(
    "Reply (%p,%p,%ld,%ld,%p) stub",
    data->lpSPMessageHeader, data->lpMessage, data->dwMessageSize,
    data->idNameServer, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Send (DPSP_SENDDATA* data) {
  g_message(
    "Send (0x%08lx,%ld,%ld,%p,%ld,%u,%p)",
    data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpMessage, data->dwMessageSize,
    data->bSystemMessage, data->lpISP
  );

  auto player = session->getPlayerById(data->idPlayerTo);
  auto stream = player->agent->getStream(1);

  stream->send(1, data->dwMessageSize, static_cast<gchar*>(data->lpMessage));

  return DP_OK;
}

struct SessionData {
  GameSession* session;
  DPID player;
};

static HRESULT WINAPI DPNice_CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  g_message(
    "CreatePlayer (%ld,0x%08lx,%p,%p)",
    data->idPlayer, data->dwFlags,
    data->lpSPMessageHeader, data->lpISP
  );

  if (data->dwFlags & DPLAYI_PLAYER_PLAYERLOCAL) {
    if (data->dwFlags & DPLAYI_PLAYER_APPSERVER) {
      return DP_OK;
    }
    // Creating our local player, register with the signaling server.
    session->getSignalingConnection()
      ->connect(session->getSessionGuid(), data->idPlayer);
  }

  return DP_OK;
}

static HRESULT WINAPI DPNice_DeletePlayer (DPSP_DELETEPLAYERDATA* data) {
  g_message(
    "DeletePlayer (%ld,0x%08lx,%p)",
    data->idPlayer, data->dwFlags, data->lpISP
  );

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

static HRESULT WINAPI DPNice_Open (DPSP_OPENDATA* data) {
  g_message(
    "Open (%u,%p,%p,%u,0x%08lx,0x%08lx)",
    data->bCreate, data->lpSPMessageHeader, data->lpISP,
    data->bReturnStatus, data->dwOpenFlags, data->dwSessionFlags
  );

  g_thread_new("main loop", &startThread, NULL);

  g_message("new session");
  auto host = const_cast<char*>(DEFAULT_SIGNALING_HOST);
  auto port = DEFAULT_SIGNALING_PORT;
  // TODO use the actual guid of the directplay session
  GUID sessionGuid;
  CoCreateGuid(&sessionGuid);
  // TODO store in DirectPlay SPData (the pointers got weird when I tried this)
  auto connection = new SignalingConnection(host, port);
  session = new GameSession(connection, sessionGuid, !!data->bCreate);

  return DP_OK;
}

static HRESULT WINAPI DPNice_CloseEx (DPSP_CLOSEDATA* data) {
  g_message("CloseEx (%p) stub", data->lpISP);
  g_main_loop_quit(gloop);
  return DPERR_UNSUPPORTED;
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

  auto guid = new wchar_t[51];
  StringFromGUID2(*spData->lpGuid, guid, 50);
  g_message("Initializing library for %ls (%ls)", guid, spData->lpszName);
  delete guid;

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
    /* TODO: Initialization */
    DisableThreadLibraryCalls(instance);
    break;
  case DLL_PROCESS_DETACH:
    break;
  default:
    break;
  }
  return TRUE;
}
