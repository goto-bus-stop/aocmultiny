#include "MainFrame.hpp"

using std::string;
using std::to_string;
using std::vector;

using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

wxListItem createListItem (int id, string content) {
  wxListItem item;
  item.SetId(id);
  item.SetText(content);
  return item;
}

MainFrame::MainFrame (const wxString& title)
    :
    wxFrame(NULL, wxID_ANY, title) {
  auto menuFile = new wxMenu;
  menuFile->Append(
    101,
    wxT("&Hello...\tCtrl-H"),
    wxT("Help string shown in status bar for this menu item")
  );
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  auto menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  auto menuBar = new wxMenuBar;
  menuBar->Append(menuFile, wxT("&File"));
  menuBar->Append(menuHelp, wxT("&Help"));

  this->gameList = new wxListView(this, wxID_ANY);
  this->gameList->InsertColumn(0, createListItem(0, "Room"));
  this->gameList->InsertColumn(1, createListItem(1, "Players"));
  this->gameList->InsertColumn(2, createListItem(2, "Description"));

  this->SetMenuBar(menuBar);
  this->CreateStatusBar();
  this->SetStatusText(wxT("Welcome to wxWidgets!"));
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MainFrame::onExit)
  EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
wxEND_EVENT_TABLE()

void MainFrame::setRooms (vector<Channel*> rooms) {
  for (auto room : rooms) {
    auto index = this->gameList->InsertItem(0, room->name);
    this->gameList->SetItem(index, 1, to_string(room->members_count));
    this->gameList->SetItem(index, 2, room->topic);
  }
}

void MainFrame::onAbout (wxCommandEvent& event) {
  wxMessageBox(
    wxT("This is a wxWidgets' Hello world sample"),
    wxT("About Hello World"),
    wxOK | wxICON_INFORMATION
  );
}

void MainFrame::onHello (wxCommandEvent& event) {
  wxLogMessage(wxT("Hello world from wxWidgets!"));
}

void MainFrame::onExit (wxCommandEvent& event) {
  this->Close(true);
}

}
}
