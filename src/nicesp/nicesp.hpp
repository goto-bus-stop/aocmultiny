#pragma once
#include "GameSession.hpp"
#include <ole2.h>
#include <dplaysp.h>
#include <gio/gnetworking.h>

#define DX61AVERSION 11
#define DPMSG_SIGNATURE 0x79616c70
#define DPMSGCMD_ENUMSESSIONS 0x0002

struct DPSP_MSG_ENVELOPE {
  DWORD magic;
  WORD commandId;
  WORD version;

  DPSP_MSG_ENVELOPE ()
      :
      magic(DPMSG_SIGNATURE),
      version(DX61AVERSION) {
  };
};

// TODO include the DirectPlay service provider header file (from Wine?) that
// has this stuff and use that instead.
struct DPSP_MSG_ENUMSESSIONS {
  DPSP_MSG_ENVELOPE envelope;
  GUID applicationGuid;
  DWORD passwordOffset;
  DWORD flags;

  DPSP_MSG_ENUMSESSIONS () {
    this->envelope.commandId = DPMSGCMD_ENUMSESSIONS;
  };
};

// {e2dd8ebe-1f03-43b7-8d92-9c6c2f5c4426}
const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

// {6f26c42e-d2ac-465a-be01-20dd2d3a5279}
const GUID DPAID_NICESignalingServer = { 0x6f26c42e, 0xd2ac, 0x465a, { 0xbe, 0x01, 0x20, 0xdd, 0x2d, 0x3a, 0x52, 0x79 } };
// {2a0ced59-3cfe-4ce7-8725-f9881463cd38}
const GUID DPAID_StunServer = { 0x2a0ced59, 0x3cfe, 0x4ce7, { 0x87, 0x25, 0xf9, 0x88, 0x14, 0x63, 0xcd, 0x38 } };

extern "C" {

__declspec(dllexport) HRESULT __cdecl SPInit (SPINITDATA* spData);

}
