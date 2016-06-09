#include "MainFrame.hpp"

namespace aocmultiny {
namespace gui {

MainFrame::MainFrame (const wxString& title)
    :
    wxFrame(NULL, wxID_ANY, title) {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(
    101,
    wxT("&Hello...\tCtrl-H"),
    wxT("Help string shown in status bar for this menu item")
  );
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, wxT("&File"));
  menuBar->Append(menuHelp, wxT("&Help"));
  this->SetMenuBar(menuBar);
  this->CreateStatusBar();
  this->SetStatusText(wxT("Welcome to wxWidgets!"));
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MainFrame::onExit)
  EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
wxEND_EVENT_TABLE()

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
