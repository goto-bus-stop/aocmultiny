#include "MainFrame.hpp"
#include "RoomFrame.hpp"

using std::string;
using std::to_string;
using std::vector;

using aocmultiny::irc::IRC;
using aocmultiny::irc::Channel;

namespace aocmultiny {
namespace gui {

MainFrame::MainFrame (const wxString& title, IRC* irc)
    :
    wxFrame(NULL, wxID_ANY, title),
    irc(irc) {
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

  auto panel = new wxPanel(this);
  auto vbox = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(vbox);

  this->roomList = new RoomList(panel);
  this->chat = new Chat(panel, this->irc, "#lobby");
  vbox->Add(this->roomList, 0, wxEXPAND, 0);
  vbox->Add(this->chat, 0, wxEXPAND, 0);

  this->SetMenuBar(menuBar);
  this->CreateStatusBar();
  this->SetStatusText(wxT("Welcome to wxWidgets!"));

  this->irc->onChannelList += [this] (auto channels) {
    this->setRooms(channels);
  };

  this->irc->list();

  this->timer = new wxTimer(this);
  this->timer->Start(5000);
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_COMMAND(ID_JOIN_ROOM, JOIN_ROOM, MainFrame::onJoinRoom)
  EVT_TIMER(wxID_ANY, MainFrame::onTimer)
  EVT_MENU(wxID_EXIT, MainFrame::onExit)
  EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
wxEND_EVENT_TABLE()

void MainFrame::setRooms (vector<Channel*> rooms) {
  this->roomList->setRooms(rooms);
}

void MainFrame::onTimer (wxTimerEvent& event) {
  this->irc->list();
}

void MainFrame::onJoinRoom (wxCommandEvent& event) {
  auto roomName = event.GetString().ToStdString();
  std::wcout << "[MainFrame::onJoinRoom] " << roomName << std::endl;

  auto room = new RoomFrame(NULL, this->irc, roomName);
  room->Show(true);
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
