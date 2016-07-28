#include "NiceAgent.hpp"
#include "nicesp.hpp"
#include <gio/gnetworking.h>
#include <stdio.h>
#include <process.h>

using namespace std;

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

namespace nicesp {

static GMainLoop* gloop;
static nicesp::NiceAgent* agent;

static HRESULT WINAPI DPNice_EnumSessions (DPSP_ENUMSESSIONSDATA* data) {
  printf("EnumSessions\n");
  printf(
    "(%p,%ld,%p,%u) stub\n",
    data->lpMessage, data->dwMessageSize,
    data->lpISP, data->bReturnStatus
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Reply (DPSP_REPLYDATA* data) {
  printf("Reply\n");
  printf(
    "(%p,%p,%ld,%ld,%p) stub\n",
    data->lpSPMessageHeader, data->lpMessage, data->dwMessageSize,
    data->idNameServer, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Send (DPSP_SENDDATA* data) {
  printf("Send\n");
  printf(
    "(0x%08lx,%ld,%ld,%p,%ld,%u,%p) stub\n",
    data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpMessage, data->dwMessageSize,
    data->bSystemMessage, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static void onCandidates (InternalAgent rawAgent, guint streamId, gpointer data) {
  printf("SIGNAL candidate gathering done\n");
  NiceAgent agent (rawAgent);
  auto sdp = agent.generateLocalStreamSdp(streamId, true);

  printf("\nSDP\n===\n");
  printf("%s\n===\n\n", sdp);
}

static void onReceive (
  InternalAgent agent,
  guint streamId,
  guint componentId,
  guint len,
  gchar* buf,
  gpointer data
) {
  printf("received %s from %d:%d\n", buf, streamId, componentId);
}

static void onStateChange (
  InternalAgent rawAgent,
  guint streamId,
  guint componentId,
  guint state,
  gpointer data
) {
  if (state == NICE_COMPONENT_STATE_READY) {
    printf("Ready %d:%d\n", streamId, componentId);
  }
}

static HRESULT WINAPI DPNice_CreatePlayer (DPSP_CREATEPLAYERDATA* data) {
  printf("CreatePlayer\n");
  printf(
    "(%ld,0x%08lx,%p,%p) stub\n",
    data->idPlayer, data->dwFlags,
    data->lpSPMessageHeader, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_DeletePlayer (DPSP_DELETEPLAYERDATA* data) {
  printf("DeletePlayer\n");
  printf(
    "(%ld,0x%08lx,%p) stub\n",
    data->idPlayer, data->dwFlags, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddress (DPSP_GETADDRESSDATA* data) {
  printf("GetAddress\n");
  printf(
    "(%ld,0x%08lx,%p,%p,%p) stub\n",
    data->idPlayer, data->dwFlags, data->lpAddress,
    data->lpdwAddressSize, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetCaps (DPSP_GETCAPSDATA* data) {
  printf("GetCaps\n");
  printf(
    "(%ld,%p,0x%08lx,%p)\n",
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

static void startThread (void*) {
  printf("loop start\n");
  g_main_loop_run(gloop);
  printf("loop complete\n");
  g_main_loop_unref(gloop);
  _endthread();
}

static HRESULT WINAPI DPNice_Open (DPSP_OPENDATA* data) {
  printf("Open\n");
  printf(
    "(%u,%p,%p,%u,0x%08lx,0x%08lx) stub\n",
    data->bCreate, data->lpSPMessageHeader, data->lpISP,
    data->bReturnStatus, data->dwOpenFlags, data->dwSessionFlags
  );

  printf("Network init\n");
  g_networking_init();

  gloop = g_main_loop_new(NULL, FALSE);

  printf("new agent\n");
  agent = new NiceAgent(g_main_loop_get_context(gloop));

  // Set the STUN settings and controlling mode
  g_object_set(agent->unwrap(), "stun-server", "stun.l.google.com", NULL);
  g_object_set(agent->unwrap(), "stun-server-port", 19302, NULL);
  g_object_set(agent->unwrap(), "controlling-mode", data->bCreate, NULL);

  // Connect to the signals
  g_signal_connect(agent->unwrap(), "candidate-gathering-done", G_CALLBACK(onCandidates), NULL);
  g_signal_connect(agent->unwrap(), "component-state-changed", G_CALLBACK(onStateChange), NULL);

  _beginthread(&startThread, 0, nullptr);

  return DP_OK;
}

static HRESULT WINAPI DPNice_CloseEx (DPSP_CLOSEDATA* data) {
  printf("CloseEx\n");
  printf("(%p) stub\n", data->lpISP);
  g_main_loop_quit(gloop);
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_ShutdownEx (DPSP_SHUTDOWNDATA* data) {
  printf("ShutdownEx\n");
  printf("(%p) stub\n", data->lpISP);
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetAddressChoices (DPSP_GETADDRESSCHOICESDATA* data) {
  printf("GetAddressChoices\n");
  printf(
    "(%p,%p,%p) stub\n",
    data->lpAddress, data->lpdwAddressSize, data->lpISP
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendEx (DPSP_SENDEXDATA* data) {
  printf("SendEx\n");
  printf(
    "(%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p,%u) stub\n",
    data->lpISP, data->dwFlags, data->idPlayerTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID, data->bSystemMessage
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_SendToGroupEx (DPSP_SENDTOGROUPEXDATA* data) {
  printf("SendToGroupEx\n");
  printf(
    "(%p,0x%08lx,%ld,%ld,%p,%ld,%ld,%ld,%ld,%p,%p) stub\n",
    data->lpISP, data->dwFlags, data->idGroupTo, data->idPlayerFrom,
    data->lpSendBuffers, data->cBuffers, data->dwMessageSize,
    data->dwPriority, data->dwTimeout, data->lpDPContext,
    data->lpdwSPMsgID
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_Cancel (DPSP_CANCELDATA* data) {
  printf("Cancel\n");
  printf(
    "(%p,0x%08lx,%p,%ld,0x%08lx,0x%08lx) stub\n",
    data->lpISP, data->dwFlags, data->lprglpvSPMsgID, data->cSPMsgID,
    data->dwMinPriority, data->dwMaxPriority
  );
  return DPERR_UNSUPPORTED;
}

static HRESULT WINAPI DPNice_GetMessageQueue (DPSP_GETMESSAGEQUEUEDATA* data) {
  printf("GetMessageQueue\n");
  printf(
    "(%p,0x%08lx,%ld,%ld,%p,%p) stub\n",
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
  printf("SPInit\n");

  auto guid = static_cast<wchar_t*>(calloc(51, sizeof(wchar_t)));
  StringFromGUID2(*spData->lpGuid, guid, 50);
  wprintf(L"Initializing library for %ls (%ls)\n", guid, spData->lpszName);
  free(guid);

  /* We only support NICE service */
  if (!IsEqualGUID(*spData->lpGuid, DPSPGUID_NICE)) {
    return DPERR_UNAVAILABLE;
  }

  /* Assign callback functions */
  setup_callbacks(spData->lpCB);

  printf("Have callbacks\n");

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
  printf("DllMain\n");
  printf("(0x%p, %ld, %p)\n", instance, reason, reserved);

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
