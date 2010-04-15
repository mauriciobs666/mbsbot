/*
 *	Copyright (C) 2010 - Mauricio Bieze Stefani
 *	This file is part of the MBSBOT project.
 *
 *	MBSBOT is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	MBSBOT is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.
 */

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
const long serialcomFrame::ID_STATICTEXT1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL6 = wxNewId();
const long serialcomFrame::ID_STATICTEXT2 = wxNewId();
const long serialcomFrame::ID_CHOICE1 = wxNewId();
const long serialcomFrame::ID_BUTTON14 = wxNewId();
const long serialcomFrame::ID_BUTTON11 = wxNewId();
const long serialcomFrame::ID_PANEL6 = wxNewId();
const long serialcomFrame::ID_BUTTON6 = wxNewId();
const long serialcomFrame::ID_BUTTON7 = wxNewId();
const long serialcomFrame::ID_BUTTON8 = wxNewId();
const long serialcomFrame::ID_PANEL3 = wxNewId();
const long serialcomFrame::ID_SLIDER1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL3 = wxNewId();
const long serialcomFrame::ID_SLIDER2 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL4 = wxNewId();
const long serialcomFrame::ID_SLIDER3 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL5 = wxNewId();
const long serialcomFrame::ID_BUTTON2 = wxNewId();
const long serialcomFrame::ID_BUTTON1 = wxNewId();
const long serialcomFrame::ID_BUTTON3 = wxNewId();
const long serialcomFrame::ID_PANEL2 = wxNewId();
const long serialcomFrame::ID_GRID1 = wxNewId();
const long serialcomFrame::ID_BUTTON4 = wxNewId();
const long serialcomFrame::ID_BUTTON5 = wxNewId();
const long serialcomFrame::ID_PANEL4 = wxNewId();
const long serialcomFrame::ID_BUTTON9 = wxNewId();
const long serialcomFrame::ID_BUTTON10 = wxNewId();
const long serialcomFrame::ID_BUTTON13 = wxNewId();
const long serialcomFrame::ID_BUTTON12 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL7 = wxNewId();
const long serialcomFrame::ID_BUTTON15 = wxNewId();
const long serialcomFrame::ID_STATICTEXT3 = wxNewId();
const long serialcomFrame::ID_STATICTEXT4 = wxNewId();
const long serialcomFrame::ID_PANEL5 = wxNewId();
const long serialcomFrame::ID_NOTEBOOK1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL1 = wxNewId();
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
    wxFlexGridSizer* FlexGridSizer4;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer3;
    wxFlexGridSizer* FlexGridSizer5;
    wxFlexGridSizer* FlexGridSizer2;
    wxStaticBoxSizer* StaticBoxSizer9;
    wxBoxSizer* BoxSizer2;
    wxFlexGridSizer* FlexGridSizer7;
    wxStaticBoxSizer* StaticBoxSizer7;
    wxStaticBoxSizer* StaticBoxSizer10;
    wxStaticBoxSizer* StaticBoxSizer8;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxBoxSizer* BoxSizer1;
    wxFlexGridSizer* FlexGridSizer6;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxStaticBoxSizer* StaticBoxSizer5;

    Create(parent, wxID_ANY, _("MBSBOT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(640,480));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Log = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("\nCopyright (C) 2010 - Mauricio Bieze Stefani\n\nMBSBOT is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nMBSBOT is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.\n"), wxDefaultPosition, wxSize(645,275), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer2->Add(Log, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    Panel6 = new wxPanel(Notebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer8 = new wxStaticBoxSizer(wxHORIZONTAL, Panel6, _("Serial Port"));
    FlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText1 = new wxStaticText(Panel6, ID_STATICTEXT1, _("Device"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer6->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl4 = new wxTextCtrl(Panel6, ID_TEXTCTRL6, _("/dev/ttyUSB0"), wxDefaultPosition, wxSize(129,25), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
    FlexGridSizer6->Add(TextCtrl4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel6, ID_STATICTEXT2, _("Speed"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer6->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice1 = new wxChoice(Panel6, ID_CHOICE1, wxDefaultPosition, wxSize(124,25), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice1->Append(_("9600"));
    Choice1->Append(_("19200"));
    Choice1->Append(_("38400"));
    Choice1->Append(_("57600"));
    Choice1->SetSelection( Choice1->Append(_("115200")) );
    FlexGridSizer6->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button14 = new wxButton(Panel6, ID_BUTTON14, _("Discard"), wxDefaultPosition, wxSize(67,29), 0, wxDefaultValidator, _T("ID_BUTTON14"));
    FlexGridSizer6->Add(Button14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button11 = new wxButton(Panel6, ID_BUTTON11, _("Set"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    FlexGridSizer6->Add(Button11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer8->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer8, 1, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel6->SetSizer(FlexGridSizer5);
    FlexGridSizer5->Fit(Panel6);
    FlexGridSizer5->SetSizeHints(Panel6);
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
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FlexGridSizer1 = new wxFlexGridSizer(1, 4, 0, 0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Left Wheel"));
    Slider1 = new wxSlider(Panel2, ID_SLIDER1, 1500, 1000, 2000, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER1"));
    StaticBoxSizer1->Add(Slider1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(Panel2, ID_TEXTCTRL3, _("1500"), wxDefaultPosition, wxSize(80,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    StaticBoxSizer1->Add(TextCtrl1, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Right Wheel"));
    Slider2 = new wxSlider(Panel2, ID_SLIDER2, 1500, 1000, 2000, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER2"));
    StaticBoxSizer2->Add(Slider2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl2 = new wxTextCtrl(Panel2, ID_TEXTCTRL4, _("1500"), wxDefaultPosition, wxSize(80,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    StaticBoxSizer2->Add(TextCtrl2, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer7 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Head"));
    Slider3 = new wxSlider(Panel2, ID_SLIDER3, 90, 0, 179, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER3"));
    StaticBoxSizer7->Add(Slider3, 1, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl3 = new wxTextCtrl(Panel2, ID_TEXTCTRL5, _("90"), wxDefaultPosition, wxSize(80,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    StaticBoxSizer7->Add(TextCtrl3, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Controls"));
    Button2 = new wxButton(Panel2, ID_BUTTON2, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticBoxSizer3->Add(Button2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel2, ID_BUTTON1, _("Set as center"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer3->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(Panel2, ID_BUTTON3, _("Center / Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer3->Add(Button3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer3, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel2->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel2);
    FlexGridSizer1->SetSizeHints(Panel2);
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    FlexGridSizer2 = new wxFlexGridSizer(1, 2, 0, 0);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Sensor Values"));
    Grid1 = new wxGrid(Panel4, ID_GRID1, wxDefaultPosition, wxSize(492,182), 0, _T("ID_GRID1"));
    Grid1->CreateGrid(6,5);
    Grid1->EnableEditing(true);
    Grid1->EnableGridLines(true);
    Grid1->SetColLabelValue(0, _("Now"));
    Grid1->SetColLabelValue(1, _("Min"));
    Grid1->SetColLabelValue(2, _("Max"));
    Grid1->SetColLabelValue(3, _("Avg"));
    Grid1->SetColLabelValue(4, _("X"));
    Grid1->SetRowLabelValue(0, _("0"));
    Grid1->SetRowLabelValue(1, _("1"));
    Grid1->SetRowLabelValue(2, _("2"));
    Grid1->SetRowLabelValue(3, _("3"));
    Grid1->SetRowLabelValue(4, _("4"));
    Grid1->SetRowLabelValue(5, _("5"));
    Grid1->SetCellValue(0, 0, _("0"));
    Grid1->SetCellValue(0, 1, _("1023"));
    Grid1->SetCellValue(0, 2, _("0"));
    Grid1->SetCellValue(0, 3, _("0"));
    Grid1->SetCellValue(0, 4, _("0"));
    Grid1->SetCellValue(1, 0, _("0"));
    Grid1->SetCellValue(1, 1, _("1023"));
    Grid1->SetCellValue(1, 2, _("0"));
    Grid1->SetCellValue(1, 3, _("0"));
    Grid1->SetCellValue(1, 4, _("0"));
    Grid1->SetCellValue(2, 0, _("0"));
    Grid1->SetCellValue(2, 1, _("1023"));
    Grid1->SetCellValue(2, 2, _("0"));
    Grid1->SetCellValue(2, 3, _("0"));
    Grid1->SetCellValue(2, 4, _("0"));
    Grid1->SetCellValue(3, 0, _("0"));
    Grid1->SetCellValue(3, 1, _("1023"));
    Grid1->SetCellValue(3, 2, _("0"));
    Grid1->SetCellValue(3, 3, _("0"));
    Grid1->SetCellValue(3, 4, _("0"));
    Grid1->SetCellValue(4, 0, _("0"));
    Grid1->SetCellValue(4, 1, _("1023"));
    Grid1->SetCellValue(4, 2, _("0"));
    Grid1->SetCellValue(4, 3, _("0"));
    Grid1->SetCellValue(4, 4, _("0"));
    Grid1->SetCellValue(5, 0, _("0"));
    Grid1->SetCellValue(5, 1, _("1023"));
    Grid1->SetCellValue(5, 2, _("0"));
    Grid1->SetCellValue(5, 3, _("0"));
    Grid1->SetCellValue(5, 4, _("0"));
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
    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    FlexGridSizer4 = new wxFlexGridSizer(3, 3, 0, 0);
    StaticBoxSizer9 = new wxStaticBoxSizer(wxHORIZONTAL, Panel5, _("Digital"));
    FlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer7->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel5, ID_BUTTON9, _("W"), wxDefaultPosition, wxSize(29,29), 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer7->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button10 = new wxButton(Panel5, ID_BUTTON10, _("A"), wxDefaultPosition, wxSize(29,29), 0, wxDefaultValidator, _T("ID_BUTTON10"));
    FlexGridSizer7->Add(Button10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button13 = new wxButton(Panel5, ID_BUTTON13, _("S"), wxDefaultPosition, wxSize(29,29), 0, wxDefaultValidator, _T("ID_BUTTON13"));
    FlexGridSizer7->Add(Button13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button12 = new wxButton(Panel5, ID_BUTTON12, _("D"), wxDefaultPosition, wxSize(29,29), 0, wxDefaultValidator, _T("ID_BUTTON12"));
    FlexGridSizer7->Add(Button12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl5 = new wxTextCtrl(Panel5, ID_TEXTCTRL7, wxEmptyString, wxDefaultPosition, wxSize(29,29), 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
    FlexGridSizer7->Add(TextCtrl5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer9->Add(FlexGridSizer7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4->Add(StaticBoxSizer9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer10 = new wxStaticBoxSizer(wxVERTICAL, Panel5, _("Joystick"));
    ButtonJoystickEn = new wxButton(Panel5, ID_BUTTON15, _("Enable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON15"));
    StaticBoxSizer10->Add(ButtonJoystickEn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel5, ID_STATICTEXT3, _("."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticBoxSizer10->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(Panel5, ID_STATICTEXT4, _("."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    StaticBoxSizer10->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4->Add(StaticBoxSizer10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel5->SetSizer(FlexGridSizer4);
    FlexGridSizer4->Fit(Panel5);
    FlexGridSizer4->SetSizeHints(Panel5);
    Notebook1->AddPage(Panel1, _("Log"), false);
    Notebook1->AddPage(Panel6, _("Setup"), false);
    Notebook1->AddPage(Panel3, _("EEPROM"), false);
    Notebook1->AddPage(Panel2, _("Servos"), false);
    Notebook1->AddPage(Panel4, _("Sensors"), false);
    Notebook1->AddPage(Panel5, _("Drive"), false);
    BoxSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SendCommandText = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(SendCommandText, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(100, false);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON14,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton14Click);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton11Click);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton6Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton7Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton8Click);
    Connect(ID_SLIDER1,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider1CmdSliderUpdated);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl1TextEnter);
    Connect(ID_SLIDER2,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider2CmdSliderUpdated);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl2TextEnter);
    Connect(ID_SLIDER3,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider3CmdSliderUpdated);
    Connect(ID_TEXTCTRL5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl3TextEnter);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton2Click);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton1Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton3Click);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton4Click);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton5Click);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton9Click);
    Connect(ID_TEXTCTRL7,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnDriveByKeyboard);
    Connect(ID_BUTTON15,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton15Click1);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&serialcomFrame::OnNotebook1PageChanged);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnSendCommandTextTextEnter);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&serialcomFrame::OnTimer1Trigger);
    //*)

	// Sensor data analysis stuff

    counter = 0;
    for(int s=0; s<6; s++)			// six sensors
		for(int c=0; c<10; c++)		// average of last (TEN) measures
		    accumulated[s][c]=0;

	// Default serial port

    if( MbsBot::getInstance()->init() == 0)
		StatusBar1->SetStatusText(_("Connected"));
    else
		StatusBar1->SetStatusText(_("Error opening serial port"));

	joystick = NULL;
}

serialcomFrame::~serialcomFrame()
{
    //(*Destroy(serialcomFrame)
    //*)
}

/* ============================================================================
 *
 *      LOG
 *
 * ============================================================================
 */

void serialcomFrame::OnSendCommandTextTextEnter(wxCommandEvent& event)
{
	wxString cmd=SendCommandText->GetLineText(0)+_("\n");

	char command[SERIAL_BUFFER_SIZE];
	strncpy(command, cmd.mb_str(wxConvUTF8), SERIAL_BUFFER_SIZE);

	MbsBot::getInstance()->send(command);

	Log->AppendText(cmd);
	SendCommandText->Clear();
}

/* ============================================================================
 *
 *      SETUP
 *
 * ============================================================================
 */

// pre-defined baud rates
const int spd[] = { 9600, 19200, 38400, 57600, 115200 };
const int n_spd = 5;

void serialcomFrame::OnButton11Click(wxCommandEvent& event)
{
    if(MbsBot::getInstance()->init(TextCtrl4->GetValue().mb_str(wxConvUTF8), spd[Choice1->GetCurrentSelection()]) == 0)
   		StatusBar1->SetStatusText(_("Connected"));
    else
		StatusBar1->SetStatusText(_("Error opening serial port"));
}

void serialcomFrame::OnButton14Click(wxCommandEvent& event)
{
   	int currSpd = MbsBot::getInstance()->getBaud();

	for(int x=0; x < n_spd; x++)
		if( currSpd == spd[x])
			Choice1->SetSelection(x);

	TextCtrl4->SetValue(wxString(MbsBot::getInstance()->getPort(), wxConvUTF8));
}

/* ============================================================================
 *
 *      EEPROM
 *
 * ============================================================================
 */

void serialcomFrame::OnButton6Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("save\n");
}

void serialcomFrame::OnButton7Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("load\n");
}

void serialcomFrame::OnButton8Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("default\n");
}

/* ============================================================================
 *
 *      SERVO CONTROL
 *
 * ============================================================================
 */

// left wheel

void serialcomFrame::OnSlider1CmdSliderUpdated(wxScrollEvent& event)
{
	int val = Slider1->GetValue();
	TextCtrl1->SetValue(wxString::Format(wxT("%i"), val));

	MbsBot::getInstance()->setLeftWheel(val);
}

void serialcomFrame::OnTextCtrl1TextEnter(wxCommandEvent& event)
{
	int val = atoi(TextCtrl1->GetValue().mb_str(wxConvUTF8));
	Slider1->SetValue(val);

	MbsBot::getInstance()->setLeftWheel(val);
}

// right wheel

void serialcomFrame::OnSlider2CmdSliderUpdated(wxScrollEvent& event)
{
	int val = Slider2->GetValue();
	TextCtrl2->SetValue(wxString::Format(wxT("%i"), val));

	MbsBot::getInstance()->setRightWheel(val);
}

void serialcomFrame::OnTextCtrl2TextEnter(wxCommandEvent& event)
{
	int val = atoi(TextCtrl2->GetValue().mb_str(wxConvUTF8));
	Slider2->SetValue(val);

	MbsBot::getInstance()->setRightWheel(val);
}

// servo "X"

void serialcomFrame::OnSlider3CmdSliderUpdated(wxScrollEvent& event)
{
	int val = Slider3->GetValue();
	TextCtrl3->SetValue(wxString::Format(wxT("%i"), val));

	MbsBot::getInstance()->setHead(val);
}

void serialcomFrame::OnTextCtrl3TextEnter(wxCommandEvent& event)
{
	int val = atoi(TextCtrl3->GetValue().mb_str(wxConvUTF8));
	Slider3->SetValue(val);

	MbsBot::getInstance()->setHead(val);
}

// Set center

void serialcomFrame::OnButton1Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->setLeftWheelCenter(Slider1->GetValue());
	MbsBot::getInstance()->setRightWheelCenter(Slider2->GetValue());
}

// Refresh

void serialcomFrame::OnButton2Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("get l\n");
	MbsBot::getInstance()->send("get r\n");
	MbsBot::getInstance()->send("get sx\n");
}

// Stop

void serialcomFrame::OnButton3Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("stop\n");
	MbsBot::getInstance()->send("get l\n");
	MbsBot::getInstance()->send("get r\n");
	MbsBot::getInstance()->send("get sx\n");
}

/* ============================================================================
 *
 *      SENSOR DATA
 *
 * ============================================================================
 */

void serialcomFrame::OnButton4Click(wxCommandEvent& event)
{
	MbsBot::getInstance()->send("get as\n");
}

void serialcomFrame::OnButton5Click(wxCommandEvent& event)
{
	for(int s = 0; s < 6; s++)
	{
		Grid1->SetCellValue (_("1023"), s, 1);
		Grid1->SetCellValue (_("0"), s, 2);
		Grid1->SetCellValue (_("0"), s, 3);
		Grid1->SetCellValue (_("0"), s, 4);
	}
}

/* ============================================================================
 *
 *      MISC
 *
 * ============================================================================
 */

void serialcomFrame::OnTimer1Trigger(wxTimerEvent& event)
{
	char *rx;
	while ((rx = MbsBot::getInstance()->receive()))
	{
		wxString str = wxString(rx,wxConvUTF8);

		//#define ALWAYS_SHOW_CMD_RECEIVED
		#ifdef ALWAYS_SHOW_CMD_RECEIVED
			Log->AppendText(str);
		#endif

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
			else if(strcmp(tok, "SX") == 0)
			{
				tok = strtok(NULL, " ");
				if (tok)
				{
					int value = atoi(tok);
					Slider3->SetValue(value);
					TextCtrl3->SetValue(wxString::Format(wxT("%i"), value));
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

						// average of last 10 measures

						accumulated[s][counter] = value;

						int average = 0;

						for (int c=0; c < 10; c++)
							average += accumulated[s][c];

						average /= 10;

						str = wxString::Format(wxT("%i"), average);
						Grid1->SetCellValue (str, s, 3);
					}
				}

				counter++;
				if(counter >= 10)
					counter = 0;
			}
			else
				Log->AppendText(str);
		}
	}
	if(joystick)
	{
        // left and right wheels in % of power

		int lw = 0;
		int rw = 0;

		wxPoint pos = joystick->GetPosition();

		if(pos.x < 0)
		{
			lw = -100;
			rw = 100;
		}
		else if(pos.x > 0)
		{
			lw = 100;
			rw = -100;
		}
		else if (pos.y < 0)
		{
			lw = 100;
			rw = 100;
		}
		else if (pos.y > 0)
		{
			lw = -100;
			rw = -100;
		}

        // debug print

		StaticText3->SetLabel(wxString::Format(wxT("%i"), lw));
		StaticText4->SetLabel(wxString::Format(wxT("%i"), rw));

        // optimization: only send command if something changed

		static int lastL = 0;
		static int lastR = 0;
		if ((lw != lastL) || (rw != lastR))
		{
			MbsBot::getInstance()->drive(lw, rw);
			lastL = lw;
			lastR = rw;
		}
	}
}

void serialcomFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
}

void serialcomFrame::OnButton9Click(wxCommandEvent& event)
{

}

void serialcomFrame::OnDriveByKeyboard(wxCommandEvent& event)
{
}

void serialcomFrame::OnButton15Click1(wxCommandEvent& event)
{
    if ( joystick == NULL)
    {
        if(wxJoystick::GetNumberJoysticks() > 0)
        {
            joystick = new wxJoystick(wxJOYSTICK1);
            if(joystick)
                ButtonJoystickEn->SetLabel(_("Disable"));
        }
    }
    else
    {
        delete joystick;
        joystick = NULL;
        ButtonJoystickEn->SetLabel(_("Enable"));
    }
}
