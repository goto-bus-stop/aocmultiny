#pragma once
#include <string>
#include <vector>
#include <wx/wx.h>

namespace aocmultiny {
namespace gui {

class MainFrame: public wxFrame {
public:
  MainFrame (const wxString& title);
  void setRooms (std::vector<std::string> room_names);
protected:
  wxDECLARE_EVENT_TABLE();
private:
  wxListBox* gameList;

  void onHello (wxCommandEvent& event);
  void onExit (wxCommandEvent& event);
  void onAbout (wxCommandEvent& event);
};

}
}
