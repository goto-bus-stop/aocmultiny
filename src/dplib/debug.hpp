#pragma once
#include <dplaysp.h>
#include <string>
#include <iostream>

using std::string;
using std::ostream;
using std::to_string;

string ptr_to_string (void* ptr) {
  char* chars = new char[12];
  sprintf(chars, "%p", ptr);
  return chars;
}

ostream& operator<<(ostream& os, DPSP_ADDPLAYERTOGROUPDATA* data) {
  return os << "AddPlayerToGroup { "
            << "player = " << to_string(data->idPlayer) << ", "
            << "group = " << to_string(data->idGroup) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_CLOSEDATA* data) {
  return os << "Close {}";
}

ostream& operator<<(ostream& os, DPSP_CREATEGROUPDATA* data) {
  return os << "CreateGroup { "
            << "group = " << to_string(data->idGroup) << ", "
            << "spMessageHeader = " << ptr_to_string(data->lpSPMessageHeader) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_CREATEPLAYERDATA* data) {
  return os << "CreatePlayer { "
    << "player = " << to_string(data->idPlayer) << ", "
    << "flags = " << to_string(data->dwFlags) << " ("
    << (data->dwFlags & DPLAYI_PLAYER_SYSPLAYER ? "PLAYER_SYSPLAYER" : "") << ", "
    << (data->dwFlags & DPLAYI_PLAYER_NAMESRVR ? "PLAYER_NAMESRVR" : "") << ", "
    << (data->dwFlags & DPLAYI_PLAYER_PLAYERINGROUP ? "PLAYER_PLAYERINGROUP" : "") << ", "
    << (data->dwFlags & DPLAYI_PLAYER_PLAYERLOCAL ? "PLAYER_PLAYERLOCAL" : "") << ", "
    << (data->dwFlags & DPLAYI_GROUP_SYSGROUP ? "GROUP_SYSGROUP" : "") << ", "
    << (data->dwFlags & DPLAYI_GROUP_DPLAYOWNS ? "GROUP_DPLAYOWNS" : "") << ", "
    << (data->dwFlags & DPLAYI_PLAYER_APPSERVER ? "PLAYER_APPSERVER" : "") << ", "
    << (data->dwFlags & DPLAYI_GROUP_HIDDEN ? "GROUP_HIDDEN" : "") << "), "
    << "serverPlayer = " << (data->dwFlags & DPPLAYER_SERVERPLAYER ? "true" : "false") << ", "
    << "spectator = " << (data->dwFlags & DPPLAYER_SPECTATOR ? "true" : "false") << ", "
    << "spMessageHeader = " << ptr_to_string(data->lpSPMessageHeader) << " "
    << "}";
}

ostream& operator<<(ostream& os, DPSP_DELETEGROUPDATA* data) {
  return os << "DeleteGroup { "
            << "group = " << to_string(data->idGroup) << ", "
            << "flags = " << to_string(data->dwFlags) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_DELETEPLAYERDATA* data) {
  return os << "DeletePlayer { "
            << "player = " << to_string(data->idPlayer) << ", "
            << "flags = " << to_string(data->dwFlags) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_ENUMSESSIONSDATA* data) {
  return os << "EnumSessions { "
            << "size = " << to_string(data->dwMessageSize) << ", "
            << "message = " << ptr_to_string(data->lpMessage) << ", "
            << "returnStatus = " << (data->bReturnStatus ? "true" : "false") << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_GETADDRESSDATA* data) {
  return os << "GetAddress { "
            << "player = " << to_string(data->idPlayer) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_GETADDRESSCHOICESDATA* data) {
  return os << "GetAddressChoices {}";
}

ostream& operator<<(ostream& os, DPSP_GETCAPSDATA* data) {
  return os << "GetCaps { "
            << "player = " << to_string(data->idPlayer) << ", "
            << "flags = " << to_string(data->dwFlags) << ", "
            << "guaranteed = " << (data->dwFlags & DPGETCAPS_GUARANTEED ? "true" : "false") << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_OPENDATA* data) {
  return os << "Open { "
            << "create = " << (data->bCreate ? "true" : "false") << ", "
            << "returnStatus = " << (data->bReturnStatus ? "true" : "false") << ", "
            << "openFlags = " << to_string(data->dwOpenFlags) << " ("
            << (data->dwOpenFlags & DPOPEN_JOIN ? "JOIN" : "") << ", "
            << (data->dwOpenFlags & DPOPEN_CREATE ? "CREATE" : "") << ", "
            << (data->dwOpenFlags & DPOPEN_RETURNSTATUS ? "RETURNSTATUS" : "") << "), "
            << "sessionFlags = " << to_string(data->dwSessionFlags) << " ("
            << (data->dwSessionFlags & DPSESSION_NEWPLAYERSDISABLED ? "NEWPLAYERSDISABLED" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_MIGRATEHOST ? "MIGRATEHOST" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_NOMESSAGEID ? "NOMESSAGEID" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_JOINDISABLED ? "JOINDISABLED" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_KEEPALIVE ? "KEEPALIVE" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_NODATAMESSAGES ? "NODATAMESSAGES" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_SECURESERVER ? "SECURESERVER" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_PRIVATE ? "PRIVATE" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_PASSWORDREQUIRED ? "PASSWORDREQUIRED" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_MULTICASTSERVER ? "MULTICASTSERVER" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_CLIENTSERVER ? "CLIENTSERVER" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_DIRECTPLAYPROTOCOL ? "DIRECTPLAYPROTOCOL" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_NOPRESERVEORDER ? "NOPRESERVEORDER" : "") << ", "
            << (data->dwSessionFlags & DPSESSION_OPTIMIZELATENCY ? "OPTIMIZELATENCY" : "") << "), "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_REMOVEPLAYERFROMGROUPDATA* data) {
  return os << "RemovePlayerFromGroup { "
            << "player = " << to_string(data->idPlayer) << ", "
            << "group = " << to_string(data->idGroup) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_REPLYDATA* data) {
  return os << "Reply { "
            << "size = " << to_string(data->dwMessageSize) << ", "
            << "message = " << ptr_to_string(data->lpMessage) << ", "
            << "nameserver = " << to_string(data->idNameServer) << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_SENDDATA* data) {
  return os << "Send { "
            << "to = " << to_string(data->idPlayerTo) << ", "
            << "from = " << to_string(data->idPlayerFrom) << ", "
            << "flags = " << to_string(data->dwFlags) << ", "
            << "size = " << to_string(data->dwMessageSize) << ", "
            << "message = " << ptr_to_string(data->lpMessage) << ", "
            << "system = " << (data->bSystemMessage ? "true" : "false") << " "
            << "}";
}

ostream& operator<<(ostream& os, DPSP_SENDTOGROUPDATA* data) {
  return os << "SendToGroup {}";
}
ostream& operator<<(ostream& os, DPSP_SENDEXDATA* data) {
  return os << "SendEx {}";
}
ostream& operator<<(ostream& os, DPSP_SENDTOGROUPEXDATA* data) {
  return os << "SendToGroupEx {}";
}
ostream& operator<<(ostream& os, DPSP_GETMESSAGEQUEUEDATA* data) {
  return os << "GetMessageQueue {}";
}

/*
typedef struct tagDPSP_SENDTOGROUPDATA
{
  DWORD          dwFlags;
  DPID           idGroupTo;
  DPID           idPlayerFrom;
  LPVOID         lpMessage;
  DWORD          dwMessageSize;
  IDirectPlaySP* lpISP;
} DPSP_SENDTOGROUPDATA, *LPDPSP_SENDTOGROUPDATA;

typedef struct tagDPSP_SENDEXDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idPlayerTo;
  DPID           idPlayerFrom;
  LPSGBUFFER     lpSendBuffers;
  DWORD          cBuffers;
  DWORD          dwMessageSize;
  DWORD          dwPriority;
  DWORD          dwTimeout;
  LPVOID         lpDPContext;
  LPDWORD        lpdwSPMsgID;
  BOOL           bSystemMessage;
} DPSP_SENDEXDATA, *LPDPSP_SENDEXDATA;

typedef struct tagDPSP_SENDTOGROUPEXDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idGroupTo;
  DPID           idPlayerFrom;
  LPSGBUFFER     lpSendBuffers;
  DWORD          cBuffers;
  DWORD          dwMessageSize;
  DWORD          dwPriority;
  DWORD          dwTimeout;
  LPVOID         lpDPContext;
  LPDWORD        lpdwSPMsgID;
} DPSP_SENDTOGROUPEXDATA, *LPDPSP_SENDTOGROUPEXDATA;

typedef struct tagDPSP_GETMESSAGEQUEUEDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idFrom;
  DPID           idTo;
  LPDWORD        lpdwNumMsgs;
  LPDWORD        lpdwNumBytes;
} DPSP_GETMESSAGEQUEUEDATA, *LPDPSP_GETMESSAGEQUEUEDATA;
*/
