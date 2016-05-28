#pragma once
#include <wx/wx.h>

namespace aocmultiny {
namespace gui {

class MainFrame: public wxFrame {
public:
  MainFrame (const wxString& title);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  void OnHello (wxCommandEvent& event);
  void OnExit (wxCommandEvent& event);
  void OnAbout (wxCommandEvent& event);

  void inputUsername ();
};

}
}
