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
  void onHello (wxCommandEvent& event);
  void onExit (wxCommandEvent& event);
  void onAbout (wxCommandEvent& event);
};

}
}
