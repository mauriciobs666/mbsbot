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
const long serialcomFrame::ID_SLIDER1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL3 = wxNewId();
const long serialcomFrame::ID_SLIDER2 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL4 = wxNewId();
const long serialcomFrame::ID_BUTTON2 = wxNewId();
const long serialcomFrame::ID_BUTTON1 = wxNewId();
const long serialcomFrame::ID_BUTTON3 = wxNewId();
const long serialcomFrame::ID_PANEL2 = wxNewId();
const long serialcomFrame::ID_BUTTON6 = wxNewId();
const long serialcomFrame::ID_BUTTON7 = wxNewId();
const long serialcomFrame::ID_BUTTON8 = wxNewId();
const long serialcomFrame::ID_PANEL3 = wxNewId();
const long serialcomFrame::ID_GRID1 = wxNewId();
const long serialcomFrame::ID_BUTTON4 = wxNewId();
const long serialcomFrame::ID_BUTTON5 = wxNewId();
const long serialcomFrame::ID_PANEL4 = wxNewId();
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
    wxStaticBoxSizer* StaticBoxSizer2;
    wxMenuItem* MenuItem2;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer3;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer2;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenu* Menu2;
    wxStaticBoxSizer* StaticBoxSizer5;

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
    FlexGridSizer1 = new wxFlexGridSizer(1, 3, 0, 0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Left Wheel"));
    Slider1 = new wxSlider(Panel2, ID_SLIDER1, 1500, 1000, 2000, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER1"));
    StaticBoxSizer1->Add(Slider1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(Panel2, ID_TEXTCTRL3, _("1500"), wxDefaultPosition, wxSize(80,21), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    StaticBoxSizer1->Add(TextCtrl1, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Right Wheel"));
    Slider2 = new wxSlider(Panel2, ID_SLIDER2, 1500, 1000, 2000, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER2"));
    StaticBoxSizer2->Add(Slider2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl2 = new wxTextCtrl(Panel2, ID_TEXTCTRL4, _("1500"), wxDefaultPosition, wxSize(80,21), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    StaticBoxSizer2->Add(TextCtrl2, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Controls"));
    Button2 = new wxButton(Panel2, ID_BUTTON2, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticBoxSizer3->Add(Button2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel2, ID_BUTTON1, _("Save as center"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer3->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(Panel2, ID_BUTTON3, _("Center / Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer3->Add(Button3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer3, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel2->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel2);
    FlexGridSizer1->SetSizeHints(Panel2);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Controls"));
    Button6 = new wxButton(Panel3, ID_BUTTON6, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    StaticBoxSizer6->Add(Button6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel3, ID_BUTTON7, _("Load"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    StaticBoxSizer6->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel3, ID_BUTTON8, _("Default"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    StaticBoxSizer6->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel3->SetSizer(FlexGridSizer3);
    FlexGridSizer3->Fit(Panel3);
    FlexGridSizer3->SetSizeHints(Panel3);
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    FlexGridSizer2 = new wxFlexGridSizer(1, 2, 0, 0);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Sensor Values"));
    Grid1 = new wxGrid(Panel4, ID_GRID1, wxDefaultPosition, wxSize(322,182), 0, _T("ID_GRID1"));
    Grid1->CreateGrid(6,3);
    Grid1->EnableEditing(true);
    Grid1->EnableGridLines(true);
    Grid1->SetColLabelValue(0, _("Current"));
    Grid1->SetColLabelValue(1, _("Min"));
    Grid1->SetColLabelValue(2, _("Max"));
    Grid1->SetRowLabelValue(0, _("0"));
    Grid1->SetRowLabelValue(1, _("1"));
    Grid1->SetRowLabelValue(2, _("2"));
    Grid1->SetRowLabelValue(3, _("3"));
    Grid1->SetRowLabelValue(4, _("4"));
    Grid1->SetRowLabelValue(5, _("5"));
    Grid1->SetCellValue(0, 0, _("0"));
    Grid1->SetCellValue(0, 1, _("1023"));
    Grid1->SetCellValue(0, 2, _("0"));
    Grid1->SetCellValue(1, 0, _("0"));
    Grid1->SetCellValue(1, 1, _("1023"));
    Grid1->SetCellValue(1, 2, _("0"));
    Grid1->SetCellValue(2, 0, _("0"));
    Grid1->SetCellValue(2, 1, _("1023"));
    Grid1->SetCellValue(2, 2, _("0"));
    Grid1->SetCellValue(3, 0, _("0"));
    Grid1->SetCellValue(3, 1, _("1023"));
    Grid1->SetCellValue(3, 2, _("0"));
    Grid1->SetCellValue(4, 0, _("0"));
    Grid1->SetCellValue(4, 1, _("1023"));
    Grid1->SetCellValue(4, 2, _("0"));
    Grid1->SetCellValue(5, 0, _("0"));
    Grid1->SetCellValue(5, 1, _("1023"));
    Grid1->SetCellValue(5, 2, _("0"));
    Grid1->SetDefaultCellFont( Grid1->GetFont() );
    Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );
    StaticBoxSizer4->Add(Grid1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(StaticBoxSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxVERTICAL, Panel4, _("Controls"));
    Button4 = new wxButton(Panel4, ID_BUTTON4, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    StaticBoxSizer5->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(Panel4, ID_BUTTON5, _("Clear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    StaticBoxSizer5->Add(Button5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(StaticBoxSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel4->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel4);
    FlexGridSizer2->SetSizeHints(Panel4);
    Notebook1->AddPage(Panel1, _("Log"), false);
    Notebook1->AddPage(Panel2, _("Servos"), false);
    Notebook1->AddPage(Panel3, _("EEPROM"), false);
    Notebook1->AddPage(Panel4, _("Sensors"), false);
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

    Connect(ID_SLIDER1,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider1CmdSliderUpdated);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl1TextEnter);
    Connect(ID_SLIDER2,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider2CmdSliderUpdated);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl2TextEnter);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton2Click);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton1Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton3Click);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton6Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton7Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton8Click);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton4Click);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton5Click);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&serialcomFrame::OnNotebook1PageChanged);
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

void serialcomFrame::OnSendCommandTextTextEnter(wxCommandEvent& event)
{
	wxString cmd=SendCommandText->GetLineText(0)+_("\n");

	char command[SERIAL_BUFFER_SIZE];
	strncpy(command, cmd.mb_str(wxConvUTF8), SERIAL_BUFFER_SIZE);

	bot.send(command);

	Log->AppendText(cmd);
	SendCommandText->Clear();
}

void serialcomFrame::OnTimer1Trigger(wxTimerEvent& event)
{
	char *rx;
	while ((rx = bot.receive()))
	{
		wxString str = wxString(rx,wxConvUTF8);
		//Log->AppendText(str);

		char * tok = strtok(rx, " ");
		if (tok)
		{
			if(strcmp(tok, "L") == 0)
			{
				tok = strtok(NULL, " ");
				if (tok)
				{
					int value = atoi(tok);
					Slider1->SetValue(value);
					TextCtrl1->SetValue(wxString::Format(wxT("%i"), value));
				}
			}
			else if(strcmp(tok, "R") == 0)
			{
				tok = strtok(NULL, " ");
				if (tok)
				{
					int value = atoi(tok);
					Slider2->SetValue(value);
					TextCtrl2->SetValue(wxString::Format(wxT("%i"), value));
				}
			}
			else if(strcmp(tok, "AS") == 0)
			{
				for(int s = 0; s < 6; s++)
				{
					tok = strtok(NULL, " ");
					if (tok)
					{
						int value = atoi(tok);

						// current value
						wxString str=wxString::Format(wxT("%i"), value);
						Grid1->SetCellValue (str, s, 0);

						// minimum value
						if(value < atoi(Grid1->GetCellValue (s, 1).mb_str(wxConvUTF8)))
							Grid1->SetCellValue (str, s, 1);

						// maximum value
						if(value > atoi(Grid1->GetCellValue (s, 2).mb_str(wxConvUTF8)))
							Grid1->SetCellValue (str, s, 2);
					}
				}
			}
			else
				Log->AppendText(str);
		}
	}
}

void serialcomFrame::OnMenuItem3Selected(wxCommandEvent& event)
{
}

void serialcomFrame::OnSlider1CmdSliderUpdated(wxScrollEvent& event)
{
	int val = Slider1->GetValue();
	TextCtrl1->SetValue(wxString::Format(wxT("%i"), val));

	bot.setLeftWheel(val);
}

void serialcomFrame::OnSlider2CmdSliderUpdated(wxScrollEvent& event)
{
	int val = Slider2->GetValue();
	TextCtrl2->SetValue(wxString::Format(wxT("%i"), val));

	bot.setRightWheel(val);
}

void serialcomFrame::OnTextCtrl1TextEnter(wxCommandEvent& event)
{
	int val = atoi(TextCtrl1->GetValue().mb_str(wxConvUTF8));
	Slider1->SetValue(val);

	bot.setLeftWheel(val);
}

void serialcomFrame::OnTextCtrl2TextEnter(wxCommandEvent& event)
{
	int val = atoi(TextCtrl2->GetValue().mb_str(wxConvUTF8));
	Slider2->SetValue(val);

	bot.setRightWheel(val);
}

void serialcomFrame::OnButton1Click(wxCommandEvent& event)
{
	bot.setLeftWheelCenter(Slider1->GetValue());
	bot.setRightWheelCenter(Slider2->GetValue());
}

void serialcomFrame::OnButton2Click(wxCommandEvent& event)
{
	bot.send("get l\n");
	bot.send("get r\n");
}

void serialcomFrame::OnButton3Click(wxCommandEvent& event)
{
	bot.send("stop\n");
	bot.send("get l\n");
	bot.send("get r\n");
}

void serialcomFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
}

void serialcomFrame::OnButton4Click(wxCommandEvent& event)
{
	bot.send("get as\n");
}

void serialcomFrame::OnButton5Click(wxCommandEvent& event)
{
	for(int s = 0; s < 6; s++)
	{
		Grid1->SetCellValue (_("1023"), s, 1);
		Grid1->SetCellValue (_("0"), s, 2);
	}
}

void serialcomFrame::OnButton6Click(wxCommandEvent& event)
{
	bot.send("save\n");
}

void serialcomFrame::OnButton7Click(wxCommandEvent& event)
{
	bot.send("load\n");
}

void serialcomFrame::OnButton8Click(wxCommandEvent& event)
{
	bot.send("default\n");
}
