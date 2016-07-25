#pragma once
#include <ole2.h>
#include <dplaysp.h>

typedef struct DPSP_MSG_HEADER {
  DWORD size;
} DPSP_MSG_HEADER;

extern "C" {

__declspec(dllexport) HRESULT __cdecl SPInit (SPINITDATA* spData);

}
