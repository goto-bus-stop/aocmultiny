#pragma once
#include <dplaysp.h>

namespace dplib {

class DPServiceProvider {
private:
  SPINITDATA* spData;
  DPSP_SPCALLBACKS* callbacks;
  IDirectPlaySP* spInstance;

public:
  /**
   * Service provider version.
   */
  DWORD version;

  /**
   * Create a DPServiceProvider instance.
   */
  DPServiceProvider (SPINITDATA* spData);

  /**
   * Get the raw DirectPlay service provider interface used by this instance.
   */
  IDirectPlaySP* getInternalServiceProvider () {
    return this->spInstance;
  }

  /**
   * Initialize the service provider so it can be used by DirectPlay.
   * The return value from this method should be used as the return value for
   * the DLL's SPInit function.
   */
  HRESULT init ();

  /**
   * Handle an incoming message.
   */
  HRESULT handleMessage (void* data, int size, void* header = nullptr);

  // Callbacks.
  virtual HRESULT EnumSessions (void* message, int size, bool returnStatus);
  virtual HRESULT EnumSessions (DPSP_ENUMSESSIONSDATA* data);
  virtual HRESULT Reply (void* spHeader, void* message, int size, DPID nameServer);
  virtual HRESULT Reply (DPSP_REPLYDATA* data);
  virtual HRESULT Send (DPSP_SENDDATA* data);
  virtual HRESULT CreatePlayer (DPSP_CREATEPLAYERDATA* data);
  virtual HRESULT DeletePlayer (DPSP_DELETEPLAYERDATA* data);
  virtual HRESULT GetAddress (DPSP_GETADDRESSDATA* data);
  virtual HRESULT GetCaps (DPSP_GETCAPSDATA* data);
  virtual HRESULT Open (DPSP_OPENDATA* data);
  virtual HRESULT Close ();
  virtual HRESULT Shutdown ();
  virtual HRESULT GetAddressChoices (DPSP_GETADDRESSCHOICESDATA* data);
  virtual HRESULT SendEx (DPSP_SENDEXDATA* data);
  virtual HRESULT SendToGroupEx (DPSP_SENDTOGROUPEXDATA* data);
  virtual HRESULT Cancel (DPSP_CANCELDATA* data);
  virtual HRESULT GetMessageQueue (DPSP_GETMESSAGEQUEUEDATA* data);

  /**
  * Get a DPServiceProvider instance from an IDirectPlaySP interface.
  */
  static DPServiceProvider* getFrom (IDirectPlaySP* sp);
};

}
