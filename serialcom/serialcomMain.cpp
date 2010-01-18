/***************************************************************
 * Name:      serialcomMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Mauricio Bieze Stefani ()
 * Created:   2010-01-07
 * Copyright: Mauricio Bieze Stefani ()
 * License:
 **************************************************************/

#include "serialcomMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(serialcomFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(serialcomFrame)
const long serialcomFrame::ID_TEXTCTRL2 = wxNewId();
const long serialcomFrame::ID_PANEL1 = wxNewId();
const long serialcomFrame::ID_PANEL2 = wxNewId();
const long serialcomFrame::ID_NOTEBOOK1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL1 = wxNewId();
const long serialcomFrame::idMenuQuit = wxNewId();
const long serialcomFrame::ID_MENUITEM1 = wxNewId();
const long serialcomFrame::idMenuAbout = wxNewId();
const long serialcomFrame::ID_STATUSBAR1 = wxNewId();
const long serialcomFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(serialcomFrame,wxFrame)
    //(*EventTable(serialcomFrame)
    //*)
END_EVENT_TABLE()

serialcomFrame::serialcomFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(serialcomFrame)
    wxMenuItem* MenuItem2;
    wxMenu* Menu3;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxMenuItem* MenuItem3;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("MBSBOT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Log = new wxTextCtrl(Panel1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(460,245), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer2->Add(Log, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
    Panel2->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel2);
    FlexGridSizer1->SetSizeHints(Panel2);
    Notebook1->AddPage(Panel1, _("Log"), false);
    Notebook1->AddPage(Panel2, _("Drive"), false);
    BoxSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SendCommandText = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(SendCommandText, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("Preferences"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuBar1->Append(Menu3, _("&Edit"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(100, false);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnSendCommandTextTextEnter);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnMenuItem3Selected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&serialcomFrame::OnTimer1Trigger);
    //*)
}

serialcomFrame::~serialcomFrame()
{
    //(*Destroy(serialcomFrame)
    //*)
}

void serialcomFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void serialcomFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

#define SERIAL_BUFFER_SIZE 100

void serialcomFrame::OnSendCommandTextTextEnter(wxCommandEvent& event)
{
	wxString cmd=SendCommandText->GetLineText(0)+_("\n");

	char command[SERIAL_BUFFER_SIZE];
	strncpy(command, cmd.mb_str(wxConvUTF8), SERIAL_BUFFER_SIZE);

	serialPort.Write(command, strlen(command));

	Log->AppendText(cmd);
	SendCommandText->Clear();
}

void serialcomFrame::OnTimer1Trigger(wxTimerEvent& event)
{
	int available;
	char response[SERIAL_BUFFER_SIZE];
	do
	{
		available = serialPort.Read(response, SERIAL_BUFFER_SIZE);
		if(available > 0)
		{
			response[available]=0;
			Log->AppendText(wxString(response,wxConvUTF8));
		}
	} while(available>0);
}

void serialcomFrame::OnMenuItem3Selected(wxCommandEvent& event)
{
}
