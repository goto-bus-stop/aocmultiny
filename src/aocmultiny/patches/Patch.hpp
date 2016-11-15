#pragma once
#include <string>
#include <wx/progdlg.h>

using std::wstring;

namespace aocmultiny {
namespace patches {

class Patch {
private:
  wxProgressDialog* progressDialog;
public:
  Patch ();
  virtual ~Patch ();

  void step (wstring title);
  void step (wstring title, int max);
  void progress ();
  void progress (int current);

  wstring getAoCDirectory ();
  wstring getHDDirectory ();
  wstring getTempDirectory ();

  virtual void install ();
};

}
}
