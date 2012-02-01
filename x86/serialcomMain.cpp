/**	Copyright (C) 2010-2011 - Mauricio Bieze Stefani
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
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(serialcomFrame)
const long serialcomFrame::ID_TEXTCTRL2 = wxNewId();
const long serialcomFrame::ID_CHECKBOX2 = wxNewId();
const long serialcomFrame::ID_BUTTON10 = wxNewId();
const long serialcomFrame::ID_PANEL1 = wxNewId();
const long serialcomFrame::ID_STATICTEXT1 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL6 = wxNewId();
const long serialcomFrame::ID_STATICTEXT2 = wxNewId();
const long serialcomFrame::ID_CHOICE1 = wxNewId();
const long serialcomFrame::ID_BUTTON14 = wxNewId();
const long serialcomFrame::ID_BUTTON11 = wxNewId();
const long serialcomFrame::ID_CHECKBOX3 = wxNewId();
const long serialcomFrame::ID_CHECKBOX1 = wxNewId();
const long serialcomFrame::ID_CHECKBOX6 = wxNewId();
const long serialcomFrame::ID_GRID2 = wxNewId();
const long serialcomFrame::ID_BUTTON9 = wxNewId();
const long serialcomFrame::ID_STATICTEXT5 = wxNewId();
const long serialcomFrame::ID_STATICTEXT4 = wxNewId();
const long serialcomFrame::ID_PANEL6 = wxNewId();
const long serialcomFrame::ID_STATICTEXT6 = wxNewId();
const long serialcomFrame::ID_CHOICE3 = wxNewId();
const long serialcomFrame::ID_STATICTEXT7 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL9 = wxNewId();
const long serialcomFrame::ID_STATICTEXT8 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL10 = wxNewId();
const long serialcomFrame::ID_STATICTEXT9 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL11 = wxNewId();
const long serialcomFrame::ID_STATICTEXT10 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL12 = wxNewId();
const long serialcomFrame::ID_STATICTEXT11 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL13 = wxNewId();
const long serialcomFrame::ID_STATICTEXT12 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL14 = wxNewId();
const long serialcomFrame::ID_BUTTON6 = wxNewId();
const long serialcomFrame::ID_BUTTON7 = wxNewId();
const long serialcomFrame::ID_BUTTON8 = wxNewId();
const long serialcomFrame::ID_BUTTON12 = wxNewId();
const long serialcomFrame::ID_PANEL3 = wxNewId();
const long serialcomFrame::ID_SLIDER1 = wxNewId();
const long serialcomFrame::ID_SLIDER2 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL3 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL4 = wxNewId();
const long serialcomFrame::ID_CHOICE2 = wxNewId();
const long serialcomFrame::ID_CHECKBOX4 = wxNewId();
const long serialcomFrame::ID_BUTTON2 = wxNewId();
const long serialcomFrame::ID_BUTTON1 = wxNewId();
const long serialcomFrame::ID_BUTTON3 = wxNewId();
const long serialcomFrame::ID_CHECKBOX5 = wxNewId();
const long serialcomFrame::ID_SLIDER4 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL7 = wxNewId();
const long serialcomFrame::ID_SLIDER3 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL5 = wxNewId();
const long serialcomFrame::ID_SLIDER5 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL8 = wxNewId();
const long serialcomFrame::ID_PANEL2 = wxNewId();
const long serialcomFrame::ID_GRID1 = wxNewId();
const long serialcomFrame::ID_CHECKBOX7 = wxNewId();
const long serialcomFrame::ID_BUTTON4 = wxNewId();
const long serialcomFrame::ID_BUTTON5 = wxNewId();
const long serialcomFrame::ID_STATICTEXT13 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL15 = wxNewId();
const long serialcomFrame::ID_PANEL4 = wxNewId();
const long serialcomFrame::ID_STATICBITMAP1 = wxNewId();
const long serialcomFrame::ID_STATICTEXT3 = wxNewId();
const long serialcomFrame::ID_PANEL5 = wxNewId();
const long serialcomFrame::ID_NOTEBOOK2 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL1 = wxNewId();
const long serialcomFrame::ID_STATUSBAR1 = wxNewId();
const long serialcomFrame::ID_TIMER1 = wxNewId();
const long serialcomFrame::ID_TOOLBAR_DEFAULT = wxNewId();
const long serialcomFrame::ID_TOOLBAR_SALVAR = wxNewId();
const long serialcomFrame::ID_TOOLBAR_CARREGAR = wxNewId();
const long serialcomFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(serialcomFrame,wxFrame)
    //(*EventTable(serialcomFrame)
    //*)
END_EVENT_TABLE()

serialcomFrame::serialcomFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(serialcomFrame)
    wxFlexGridSizer* FlexGridSizer4;
    wxStaticBoxSizer* StaticBoxSizer12;
    wxStaticBoxSizer* StaticBoxSizer14;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer10;
    wxFlexGridSizer* FlexGridSizer3;
    wxFlexGridSizer* FlexGridSizer5;
    wxFlexGridSizer* FlexGridSizer9;
    wxFlexGridSizer* FlexGridSizer2;
    wxStaticBoxSizer* StaticBoxSizer9;
    wxBoxSizer* BoxSizer2;
    wxFlexGridSizer* FlexGridSizer7;
    wxStaticBoxSizer* StaticBoxSizer7;
    wxStaticBoxSizer* StaticBoxSizer13;
    wxStaticBoxSizer* StaticBoxSizer10;
    wxStaticBoxSizer* StaticBoxSizer8;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxFlexGridSizer* FlexGridSizer8;
    wxStaticBoxSizer* StaticBoxSizer11;
    wxBoxSizer* BoxSizer1;
    wxFlexGridSizer* FlexGridSizer13;
    wxFlexGridSizer* FlexGridSizer12;
    wxFlexGridSizer* FlexGridSizer6;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer11;
    wxStaticBoxSizer* StaticBoxSizer5;

    Create(parent, wxID_ANY, _("MBSBOT - (c) 2010-2012 GPL - Mauricio Bieze Stefani"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(640,480));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK2, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK2"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Log = new wxTextCtrl(Panel1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(500,275), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer2->Add(Log, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    StaticBoxSizer9 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("Log"));
    FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
    CheckBoxRXdata = new wxCheckBox(Panel1, ID_CHECKBOX2, _("Dump RX"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxRXdata->SetValue(false);
    FlexGridSizer8->Add(CheckBoxRXdata, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button10 = new wxButton(Panel1, ID_BUTTON10, _("Limpar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON10"));
    FlexGridSizer8->Add(Button10, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer9->Add(FlexGridSizer8, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer9, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    Panel6 = new wxPanel(Notebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer8 = new wxStaticBoxSizer(wxHORIZONTAL, Panel6, _("Serial"));
    FlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText1 = new wxStaticText(Panel6, ID_STATICTEXT1, _("Dispositivo"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer6->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl4 = new wxTextCtrl(Panel6, ID_TEXTCTRL6, _("/dev/ttyUSB0"), wxDefaultPosition, wxSize(129,25), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
    FlexGridSizer6->Add(TextCtrl4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel6, ID_STATICTEXT2, _("Baud (bps)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer6->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice1 = new wxChoice(Panel6, ID_CHOICE1, wxDefaultPosition, wxSize(124,25), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice1->Append(_("9600"));
    Choice1->Append(_("19200"));
    Choice1->Append(_("38400"));
    Choice1->Append(_("57600"));
    Choice1->SetSelection( Choice1->Append(_("115200")) );
    FlexGridSizer6->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button14 = new wxButton(Panel6, ID_BUTTON14, _("Cancelar"), wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_BUTTON14"));
    FlexGridSizer6->Add(Button14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button11 = new wxButton(Panel6, ID_BUTTON11, _("Conectar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    FlexGridSizer6->Add(Button11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer8->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer8, 1, wxALL|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer13 = new wxStaticBoxSizer(wxVERTICAL, Panel6, _("Joystick"));
    FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer10 = new wxFlexGridSizer(0, 3, 0, 0);
    CheckBoxEnJoystick = new wxCheckBox(Panel6, ID_CHECKBOX3, _("Habilitar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    CheckBoxEnJoystick->SetValue(false);
    FlexGridSizer10->Add(CheckBoxEnJoystick, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxDrvByJoy = new wxCheckBox(Panel6, ID_CHECKBOX1, _("Drive by"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxDrvByJoy->SetValue(true);
    FlexGridSizer10->Add(CheckBoxDrvByJoy, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxJoyServos = new wxCheckBox(Panel6, ID_CHECKBOX6, _("Servos"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    CheckBoxJoyServos->SetValue(false);
    FlexGridSizer10->Add(CheckBoxJoyServos, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(FlexGridSizer10, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    GridJoy = new wxGrid(Panel6, ID_GRID2, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GRID2"));
    GridJoy->CreateGrid(6,4);
    GridJoy->EnableEditing(false);
    GridJoy->EnableGridLines(true);
    GridJoy->SetColLabelValue(0, _("Atual"));
    GridJoy->SetColLabelValue(1, _("Min"));
    GridJoy->SetColLabelValue(2, _("Centro"));
    GridJoy->SetColLabelValue(3, _("Max"));
    GridJoy->SetRowLabelValue(0, _("X"));
    GridJoy->SetRowLabelValue(1, _("Y"));
    GridJoy->SetRowLabelValue(2, _("Z"));
    GridJoy->SetRowLabelValue(3, _("U"));
    GridJoy->SetRowLabelValue(4, _("V"));
    GridJoy->SetRowLabelValue(5, _("Rudder"));
    GridJoy->SetDefaultCellFont( GridJoy->GetFont() );
    GridJoy->SetDefaultCellTextColour( GridJoy->GetForegroundColour() );
    FlexGridSizer7->Add(GridJoy, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer9 = new wxFlexGridSizer(1, 4, 0, 0);
    Button9 = new wxButton(Panel6, ID_BUTTON9, _("Centrar"), wxDefaultPosition, wxSize(127,27), 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer9->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer9->Add(73,20,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(Panel6, ID_STATICTEXT5, _("Botoes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer9->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextJoyButtons = new wxStaticText(Panel6, ID_STATICTEXT4, _("0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer9->Add(StaticTextJoyButtons, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(FlexGridSizer9, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer13->Add(FlexGridSizer7, 1, wxALL|wxEXPAND|wxSHAPED|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer13, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel6->SetSizer(FlexGridSizer5);
    FlexGridSizer5->Fit(Panel6);
    FlexGridSizer5->SetSizeHints(Panel6);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer12 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Variaveis"));
    FlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText5 = new wxStaticText(Panel3, ID_STATICTEXT6, _("Programa:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer12->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoicePrg = new wxChoice(Panel3, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
    ChoicePrg->Append(_("PRG_RC"));
    ChoicePrg->Append(_("PRG_SHOW_SENSORS"));
    ChoicePrg->Append(_("PRG_PHOTOVORE"));
    ChoicePrg->Append(_("PRG_LINEFOLLOWER"));
    ChoicePrg->Append(_("PRG_SHARP"));
    ChoicePrg->Append(_("PRG_SHARP_CHASE"));
    ChoicePrg->Append(_("PRG_COLLISION"));
    ChoicePrg->Append(_("PRG_SENTRY"));
    ChoicePrg->Append(_("PRG_WIICHUCK"));
    ChoicePrg->Append(_("PRG_SCOPE"));
    ChoicePrg->Append(_("PRG_KNOB"));
    ChoicePrg->Append(_("PRG_TEST"));
    FlexGridSizer12->Add(ChoicePrg, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel3, ID_STATICTEXT7, _("Delay pol:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer12->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayInch = new wxTextCtrl(Panel3, ID_TEXTCTRL9, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
    FlexGridSizer12->Add(TextCtrlDelayInch, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel3, ID_STATICTEXT8, _("Delay 90:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer12->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayTurn = new wxTextCtrl(Panel3, ID_TEXTCTRL10, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
    FlexGridSizer12->Add(TextCtrlDelayTurn, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(Panel3, ID_STATICTEXT9, _("Delay RF:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer12->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayRead = new wxTextCtrl(Panel3, ID_TEXTCTRL11, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL11"));
    FlexGridSizer12->Add(TextCtrlDelayRead, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer12->Add(FlexGridSizer12, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer13 = new wxFlexGridSizer(0, 6, 0, 0);
    StaticText9 = new wxStaticText(Panel3, ID_STATICTEXT10, _("P"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer13->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKp = new wxTextCtrl(Panel3, ID_TEXTCTRL12, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL12"));
    FlexGridSizer13->Add(TextCtrlKp, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(Panel3, ID_STATICTEXT11, _("I"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer13->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKi = new wxTextCtrl(Panel3, ID_TEXTCTRL13, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL13"));
    FlexGridSizer13->Add(TextCtrlKi, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText11 = new wxStaticText(Panel3, ID_STATICTEXT12, _("D"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer13->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKd = new wxTextCtrl(Panel3, ID_TEXTCTRL14, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL14"));
    FlexGridSizer13->Add(TextCtrlKd, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer12->Add(FlexGridSizer13, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer12, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Controls"));
    Button6 = new wxButton(Panel3, ID_BUTTON6, _("Salvar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    StaticBoxSizer6->Add(Button6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel3, ID_BUTTON7, _("Carregar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    StaticBoxSizer6->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel3, ID_BUTTON8, _("Default"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    StaticBoxSizer6->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button12 = new wxButton(Panel3, ID_BUTTON12, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON12"));
    StaticBoxSizer6->Add(Button12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer6, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel3->SetSizer(FlexGridSizer3);
    FlexGridSizer3->Fit(Panel3);
    FlexGridSizer3->SetSizeHints(Panel3);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FlexGridSizer1 = new wxFlexGridSizer(1, 6, 0, 0);
    StaticBoxSizer14 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Rodas"));
    FlexGridSizer11 = new wxFlexGridSizer(2, 2, 0, 0);
    Slider1 = new wxSlider(Panel2, ID_SLIDER1, 0, -255, 255, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER1"));
    FlexGridSizer11->Add(Slider1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Slider2 = new wxSlider(Panel2, ID_SLIDER2, 0, -255, 255, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER2"));
    FlexGridSizer11->Add(Slider2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(Panel2, ID_TEXTCTRL3, _("0"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer11->Add(TextCtrl1, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl2 = new wxTextCtrl(Panel2, ID_TEXTCTRL4, _("0"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer11->Add(TextCtrl2, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer14->Add(FlexGridSizer11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice2 = new wxChoice(Panel2, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    Choice2->Append(_("Servo"));
    Choice2->SetSelection( Choice2->Append(_("PWM")) );
    StaticBoxSizer14->Add(Choice2, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer14, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Controles"));
    CheckBoxAutoRefresh = new wxCheckBox(Panel2, ID_CHECKBOX4, _("Auto-refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    CheckBoxAutoRefresh->SetValue(false);
    StaticBoxSizer3->Add(CheckBoxAutoRefresh, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(Panel2, ID_BUTTON2, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticBoxSizer3->Add(Button2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel2, ID_BUTTON1, _("Salvar centro"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer3->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(Panel2, ID_BUTTON3, _("Parar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer3->Add(Button3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxHandBrake = new wxCheckBox(Panel2, ID_CHECKBOX5, _("Freio de mao"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    CheckBoxHandBrake->SetValue(true);
    StaticBoxSizer3->Add(CheckBoxHandBrake, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Servos"));
    StaticBoxSizer10 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Tilt"));
    SliderTilt = new wxSlider(Panel2, ID_SLIDER4, 90, 5, 175, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL, wxDefaultValidator, _T("ID_SLIDER4"));
    StaticBoxSizer10->Add(SliderTilt, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlTilt = new wxTextCtrl(Panel2, ID_TEXTCTRL7, _("90"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL7"));
    StaticBoxSizer10->Add(TextCtrlTilt, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(StaticBoxSizer10, 1, wxEXPAND|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer7 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("Pan"));
    SliderPan = new wxSlider(Panel2, ID_SLIDER3, 90, 5, 175, wxDefaultPosition, wxSize(150,25), wxSL_HORIZONTAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER3"));
    StaticBoxSizer7->Add(SliderPan, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPan = new wxTextCtrl(Panel2, ID_TEXTCTRL5, _("90"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    StaticBoxSizer7->Add(TextCtrlPan, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(StaticBoxSizer7, 0, wxEXPAND|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer11 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("Roll"));
    SliderRoll = new wxSlider(Panel2, ID_SLIDER5, 90, 5, 175, wxDefaultPosition, wxSize(150,25), wxSL_HORIZONTAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER5"));
    StaticBoxSizer11->Add(SliderRoll, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlRoll = new wxTextCtrl(Panel2, ID_TEXTCTRL8, _("90"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL8"));
    StaticBoxSizer11->Add(TextCtrlRoll, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(StaticBoxSizer11, 0, wxEXPAND|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel2->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel2);
    FlexGridSizer1->SetSizeHints(Panel2);
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    FlexGridSizer2 = new wxFlexGridSizer(2, 1, 0, 0);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Entradas analogicas"));
    Grid1 = new wxGrid(Panel4, ID_GRID1, wxDefaultPosition, wxSize(492,182), 0, _T("ID_GRID1"));
    Grid1->CreateGrid(6,5);
    Grid1->EnableEditing(true);
    Grid1->EnableGridLines(true);
    Grid1->SetColLabelValue(0, _("Agora"));
    Grid1->SetColLabelValue(1, _("Min"));
    Grid1->SetColLabelValue(2, _("Max"));
    Grid1->SetColLabelValue(3, _("Media/10"));
    Grid1->SetColLabelValue(4, _("f(X)"));
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
    FlexGridSizer2->Add(StaticBoxSizer4, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Controles"));
    CheckBoxAutoRefreshSensors = new wxCheckBox(Panel4, ID_CHECKBOX7, _("Auto-refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    CheckBoxAutoRefreshSensors->SetValue(false);
    StaticBoxSizer5->Add(CheckBoxAutoRefreshSensors, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(Panel4, ID_BUTTON4, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    StaticBoxSizer5->Add(Button4, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(Panel4, ID_BUTTON5, _("Limpar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    StaticBoxSizer5->Add(Button5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText12 = new wxStaticText(Panel4, ID_STATICTEXT13, _("f(X) = "), wxDefaultPosition, wxSize(-1,-1), 0, _T("ID_STATICTEXT13"));
    StaticBoxSizer5->Add(StaticText12, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl3 = new wxTextCtrl(Panel4, ID_TEXTCTRL15, _("x"), wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL15"));
    StaticBoxSizer5->Add(TextCtrl3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(StaticBoxSizer5, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel4->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel4);
    FlexGridSizer2->SetSizeHints(Panel4);
    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    FlexGridSizer4 = new wxFlexGridSizer(3, 1, 0, 0);
    StaticBitmap1 = new wxStaticBitmap(Panel5, ID_STATICBITMAP1, wxBitmap(wxImage(_T("m2d2.jpg"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    FlexGridSizer4->Add(StaticBitmap1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel5, ID_STATICTEXT3, _("\nCopyright (C) 2010-2012 - Mauricio Bieze Stefani\n\nMBSBOT is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nMBSBOT is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.\n"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer4->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    Panel5->SetSizer(FlexGridSizer4);
    FlexGridSizer4->Fit(Panel5);
    FlexGridSizer4->SetSizeHints(Panel5);
    Notebook1->AddPage(Panel1, _("Log"), false);
    Notebook1->AddPage(Panel6, _("Local"), false);
    Notebook1->AddPage(Panel3, _("EEPROM"), false);
    Notebook1->AddPage(Panel2, _("Motores"), false);
    Notebook1->AddPage(Panel4, _("Sensores"), false);
    Notebook1->AddPage(Panel5, _("GPL"), false);
    BoxSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SendCommandText = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(SendCommandText, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[3] = { -10, -10, -10 };
    int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(3,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(3,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(100, false);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(ID_TOOLBAR_DEFAULT, _("Novo"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Carrega defaults"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(ID_TOOLBAR_SALVAR, _("Salvar"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Salva dados na EEPROM"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(ID_TOOLBAR_CARREGAR, _("Carregar"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Carrega config da EEPROM"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton10Click);
    Connect(ID_BUTTON14,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton14Click);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton11Click);
    Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnCheckBoxJoystick);
    Connect(ID_GRID2,wxEVT_GRID_CELL_LEFT_CLICK,(wxObjectEventFunction)&serialcomFrame::OnGridJoyCellLeftClick);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton9Click);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnChoiceProgram);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton6Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton7Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton8Click);
    Connect(ID_BUTTON12,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton12Click);
    Connect(ID_SLIDER1,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider1CmdSliderUpdated);
    Connect(ID_SLIDER2,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider2CmdSliderUpdated);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl1TextEnter);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl2TextEnter);
    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnChoiceDCServo);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton2Click);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton1Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton3Click);
    Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnCheckBoxHandBrakeClick);
    Connect(ID_SLIDER4,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSliderTiltCmdSliderUpdated);
    Connect(ID_TEXTCTRL7,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlTiltTextEnter);
    Connect(ID_SLIDER3,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider3CmdSliderUpdated);
    Connect(ID_TEXTCTRL5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl3TextEnter);
    Connect(ID_SLIDER5,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSliderRollCmdSliderUpdated);
    Connect(ID_TEXTCTRL8,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlRollTextEnter);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton4Click);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton5Click);
    Connect(ID_NOTEBOOK2,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&serialcomFrame::OnNotebook1PageChanged);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnSendCommandTextTextEnter);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&serialcomFrame::OnTimer1Trigger);
    //*)

    // Default serial port

    if( MbsBot::getInstance()->init() == 0)
        StatusBar1->SetStatusText(_("Conectado"));
    else
        StatusBar1->SetStatusText(_("Erro na porta serial"));

    int currSpd = MbsBot::getInstance()->getBaud();
    for(int x=0; x < n_spd; x++)
        if( currSpd == spd[x])
            Choice1->SetSelection(x);
    TextCtrl4->SetValue(wxString(MbsBot::getInstance()->getPort(), wxConvUTF8));

    joystick = NULL;
}

serialcomFrame::~serialcomFrame()
{
    //(*Destroy(serialcomFrame)
    //*)
}

// ============================================================================
//      LOG
// ============================================================================

void serialcomFrame::OnSendCommandTextTextEnter(wxCommandEvent& event)
{
    wxString wx_cmd=SendCommandText->GetLineText(0);
    Log->AppendText(wx_cmd+_("\n"));
    SendCommandText->Clear();

    char cmd[MAX_CMD];
    strncpy(cmd, wx_cmd.mb_str(wxConvUTF8), MAX_CMD -1);

    MbsBot::getInstance()->envia("%s%c", cmd, CMD_EOL);
}

// ============================================================================
//      SETUP
// ============================================================================

void serialcomFrame::OnButton11Click(wxCommandEvent& event)
{
    if(MbsBot::getInstance()->init(TextCtrl4->GetValue().mb_str(wxConvUTF8), spd[Choice1->GetCurrentSelection()]) == 0)
    {
        StatusBar1->SetStatusText(_("Conectado"));
    }
    else
        StatusBar1->SetStatusText(_("Erro na porta serial"));
}
void serialcomFrame::OnButton14Click(wxCommandEvent& event)
{
    int currSpd = MbsBot::getInstance()->getBaud();

    for(int x=0; x < n_spd; x++)
        if( currSpd == spd[x])
            Choice1->SetSelection(x);

    TextCtrl4->SetValue(wxString(MbsBot::getInstance()->getPort(), wxConvUTF8));
}

// ============================================================================
//      EEPROM
// ============================================================================

void serialcomFrame::OnButton6Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->save();
}
void serialcomFrame::OnButton7Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->load();
}
void serialcomFrame::OnButton8Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->loadDefault();
}

// ============================================================================
//      MOTORS
// ============================================================================

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

// Pan servo

void serialcomFrame::OnSlider3CmdSliderUpdated(wxScrollEvent& event)
{
    int val = SliderPan->GetValue();
    TextCtrlPan->SetValue(wxString::Format(wxT("%i"), val));

    MbsBot::getInstance()->setPan(val);
}
void serialcomFrame::OnTextCtrl3TextEnter(wxCommandEvent& event)
{
    int val = atoi(TextCtrlPan->GetValue().mb_str(wxConvUTF8));
    SliderPan->SetValue(val);

    MbsBot::getInstance()->setPan(val);
}

// Tilt servo

void serialcomFrame::OnSliderTiltCmdSliderUpdated(wxScrollEvent& event)
{
    int val = SliderTilt->GetValue();
    TextCtrlTilt->SetValue(wxString::Format(wxT("%i"), val));

    MbsBot::getInstance()->setTilt(val);
}
void serialcomFrame::OnTextCtrlTiltTextEnter(wxCommandEvent& event)
{
    int val = atoi(TextCtrlTilt->GetValue().mb_str(wxConvUTF8));
    SliderTilt->SetValue(val);

    MbsBot::getInstance()->setTilt(val);
}

// Roll servo

void serialcomFrame::OnSliderRollCmdSliderUpdated(wxScrollEvent& event)
{
    int val = SliderRoll->GetValue();
    TextCtrlRoll->SetValue(wxString::Format(wxT("%i"), val));

    MbsBot::getInstance()->setRoll(val);
}
void serialcomFrame::OnTextCtrlRollTextEnter(wxCommandEvent& event)
{
    int val = atoi(TextCtrlRoll->GetValue().mb_str(wxConvUTF8));
    SliderRoll->SetValue(val);

    MbsBot::getInstance()->setRoll(val);
}

// Set center

void serialcomFrame::OnButton1Click(wxCommandEvent& event)
{
    if(wxYES == wxMessageBox(_("R U sure?"), _("OVERWRITE"), wxYES_NO))
    {
        MbsBot::getInstance()->setLeftWheelCenter(Slider1->GetValue());
        MbsBot::getInstance()->setRightWheelCenter(Slider2->GetValue());
    }
}

// Refresh

void serialcomFrame::OnButton2Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVar(VAR_RODA_ESQ);
    MbsBot::getInstance()->pedeVar(VAR_RODA_DIR);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_X);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_Y);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_Z);
}

// Stop

void serialcomFrame::OnButton3Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->stop();
    MbsBot::getInstance()->pedeVar(VAR_RODA_ESQ);
    MbsBot::getInstance()->pedeVar(VAR_RODA_DIR);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_X);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_Y);
    MbsBot::getInstance()->pedeVar(VAR_SERVO_Z);
}

// ============================================================================
//      SENSOR DATA
// ============================================================================

void serialcomFrame::OnButton4Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVar(VAR_AS);  // todas entradas analogicas An
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

// ============================================================================
//      MISC
// ============================================================================

void serialcomFrame::OnTimer1Trigger(wxTimerEvent& event)
{
    static char onceInASecond = 10;
    if(onceInASecond == 0)
    {
        if(CheckBoxAutoRefresh->IsChecked())
            MbsBot::getInstance()->status();
        if(CheckBoxAutoRefreshSensors->IsChecked())
            MbsBot::getInstance()->pedeVar(VAR_AS);
        onceInASecond = 10;
    }
    else
        onceInASecond--;

    char *rx;
    while ((rx = MbsBot::getInstance()->receive()))
    {
        wxString str = wxString(rx,wxConvUTF8);

        if(CheckBoxRXdata->IsChecked())
            Log->AppendText(str);

        char * tok = strtok(rx, " ");
        if (tok)
        {
            if(strcmp(tok, "L") == 0)		// left wheel
            {
                tok = strtok(NULL, " ");
                if (tok)
                {
                    int value = atoi(tok);
                    Slider1->SetValue(value);
                    TextCtrl1->SetValue(wxString::Format(wxT("%i"), value));
                }
            }
            else if(strcmp(tok, "R") == 0)	// right wheel
            {
                tok = strtok(NULL, " ");
                if (tok)
                {
                    int value = atoi(tok);
                    Slider2->SetValue(value);
                    TextCtrl2->SetValue(wxString::Format(wxT("%i"), value));
                }
            }
            else if(strcmp(tok, "SX") == 0)	// head servo
            {
                tok = strtok(NULL, " ");
                if (tok)
                {
                    int value = atoi(tok);
                    SliderPan->SetValue(value);
                    TextCtrlPan->SetValue(wxString::Format(wxT("%i"), value));
                }
            }
            else if(strcmp(tok, "AS") == 0)	// all analog sensors
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
                        sensorAvg[s].read(value);
                        str = wxString::Format(wxT("%i"), sensorAvg[s].getAverage());
                        Grid1->SetCellValue (str, s, 3);
                    }
                }
            }
            else if(strcmp(tok, "S") == 0)	// status
            {
                StatusBar1->SetStatusText(str);

                tok = strtok(NULL, " ");
                if (tok)
                {
                    // current program
                    int value = atoi(tok);
                    ChoicePrg->SetSelection(value);

                    tok = strtok(NULL, " ");
                    if (tok)
                    {
                        // last error
                        int value = atoi(tok);

                        tok = strtok(NULL, " ");
                        if (tok)
                        {
                            // left wheel
                            value = atoi(tok);
                            Slider1->SetValue(value);
                            TextCtrl1->SetValue(wxString::Format(wxT("%i"), value));

                            tok = strtok(NULL, " ");
                            if (tok)
                            {
                                // right wheel
                                value = atoi(tok);
                                Slider2->SetValue(value);
                                TextCtrl2->SetValue(wxString::Format(wxT("%i"), value));

                                tok = strtok(NULL, " ");
                                if (tok)
                                {
                                    // Pan servo
                                    value = atoi(tok);
                                    SliderPan->SetValue(value);
                                    TextCtrlPan->SetValue(wxString::Format(wxT("%i"), value));

                                    tok = strtok(NULL, " ");
                                    if (tok)
                                    {
                                        // Tilt servo
                                        value = atoi(tok);
                                        SliderTilt->SetValue(value);
                                        TextCtrlTilt->SetValue(wxString::Format(wxT("%i"), value));

                                        tok = strtok(NULL, " ");
                                        if (tok)
                                        {
                                            // Roll servo
                                            value = atoi(tok);
                                            SliderRoll->SetValue(value);
                                            TextCtrlRoll->SetValue(wxString::Format(wxT("%i"), value));
                                            //StatusBar1->SetStatusText(_("Status OK"));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(strcmp(tok, "DI") == 0)	// delay inch
            {
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlDelayInch->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
            }
            else if(strcmp(tok, "DR") == 0)	// delay right turn
            {
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlDelayTurn->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
            }
            else if(strcmp(tok, "DRF") == 0) // delay range finder reads
            {
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlDelayRead->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
            }
            else if(strcmp(tok, "PID") == 0)	// delay inch
            {
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlKp->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlKi->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
                tok = strtok(NULL, " ");
                if (tok)
                    TextCtrlKd->SetValue(wxString::Format(wxT("%i"), atoi(tok)));
            }
            else if(!CheckBoxRXdata->IsChecked())
                Log->AppendText(str);
        }
    }

    if(joystick)
    {
        int botoesAgora = joystick->GetButtonState();
        StaticTextJoyButtons->SetLabel(wxString::Format(wxT("%i"), botoesAgora));

        #define BT_90_ESQ 0x010
        #define BT_90_DIR 0x020
        #define BT_SELECT 0x100
        #define BT_START  0x200

        int clicados = (botoesAgora ^ botoesAntes) & botoesAgora;

        // vira 90 graus pra esquerda
        if(clicados & BT_90_ESQ)
        {
            MbsBot::getInstance()->sqrLeft();
        }
        // vira 90 graus pra direita
        if(clicados & BT_90_DIR)
        {
            MbsBot::getInstance()->sqrRight();
        }

        if(clicados & BT_SELECT)
        {
            MbsBot::getInstance()->enviaVar(VAR_FREIO, 1);
            CheckBoxDrvByJoy->SetValue(false);
            CheckBoxJoyServos->SetValue(true);
        }

        if(clicados & BT_START)
        {
            // solta freio de mao
            MbsBot::getInstance()->enviaVar(VAR_FREIO, 0);

            // auto calibra centro joystick
            joyCenter = joystick->GetPosition();
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.x), 0, 2);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.y), 1, 2);

            CheckBoxDrvByJoy->SetValue(true);
            CheckBoxJoyServos->SetValue(false);
        }

        if(clicados & 0x001)
        {
            MbsBot::getInstance()->setProgram(PRG_TEST);
        }
        if(clicados & 0x002)
        {
            // direita
        }
        if(clicados & 0x004)
        {
            MbsBot::getInstance()->stop();
        }
        if(clicados & 0x008)
        {
            // esquerda
        }
        if(clicados & 0x040)
        {
        }
        if(clicados & 0x080)
        {
        }
        if(clicados & 0x400)
        {
        }
        if(clicados & 0x800)
        {
        }

        botoesAntes = botoesAgora;

        // Current position

        joyPos = joystick->GetPosition();
        // x < centro = esquerda
        // x > centro = direita
        // y < centro = p/ cima
        // y > centro = p/ baixo

        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyPos.x), 0, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyPos.y), 1, 0);

        // Lembra maximo absoluto

        if(joyPos.x > joyMax.x)
        {
            joyMax.x = joyPos.x;
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMax.x), 0, 3);
        }

        if(joyPos.y > joyMax.y)
        {
            joyMax.y = joyPos.y;
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMax.y), 1, 3);
        }

        // Lembra minimo absoluto

        if(joyPos.x < joyMin.x)
        {
            joyMin.x = joyPos.x;
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMin.x), 0, 1);
        }

        if(joyPos.y < joyMin.y)
        {
            joyMin.y = joyPos.y;
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMin.y), 1, 1);
        }

        // Direcao vetorial

        if(CheckBoxDrvByJoy->IsChecked())
        {
            int x = ((joyPos.x - joyCenter.x) * 110) / ((joyMax.x - joyMin.x) / 2);
            int y = ((joyCenter.y - joyPos.y) * 110) / ((joyMax.y - joyMin.y) / 2); // y eh invertido

            MbsBot::getInstance()->vectorialDrive(x,y);
        }

        // Servos Pan Tilt Roll

        if(CheckBoxJoyServos->IsChecked())
        {
            // TODO (mbs#1#): Controlar os Servos via joystick

            int x = ((joyPos.x - joyCenter.x) * 100) / ((joyMax.x - joyMin.x) / 2);
            int y = ((joyCenter.y - joyPos.y) * 100) / ((joyMax.y - joyMin.y) / 2); // y eh invertido
        }

        if(joystick->HasZ())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetZPosition()), 2, 0);
        }
        if(joystick->HasU())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetUPosition()), 3, 0);
        }
        if(joystick->HasV())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetVPosition()), 4, 0);
        }
    }
}

void serialcomFrame::OnChoiceDCServo(wxCommandEvent& event)
{
    if( Choice2->GetCurrentSelection() == 0 )
    {
        // Wheel Servo
        Slider1->SetRange(1000,2000);
        Slider2->SetRange(1000,2000);
    }
    else if ( Choice2->GetCurrentSelection() == 1 )
    {
        // Wheel PWM
        Slider1->SetRange(-255,255);
        Slider2->SetRange(-255,255);
    }
    MbsBot::getInstance()->pedeVar(VAR_RODA_ESQ);
    MbsBot::getInstance()->pedeVar(VAR_RODA_DIR);
}

void serialcomFrame::OnChoiceProgram(wxCommandEvent& event)
{
    MbsBot::getInstance()->setProgram((enum ProgramID)(ChoicePrg->GetCurrentSelection()));
    MbsBot::getInstance()->stop();
}

void serialcomFrame::OnCheckBoxJoystick(wxCommandEvent& event)
{
    if ( joystick != NULL)
    {
        delete joystick;
        joystick = NULL;
    }

    if(CheckBoxEnJoystick->IsChecked())
    {
        int NumberJoysticks = wxJoystick::GetNumberJoysticks();

        if ( NumberJoysticks <= 0 )
        {
            CheckBoxEnJoystick->SetValue(false);
            wxMessageBox(_("No joysticks available"));
            Log->AppendText(_("No joysticks available\n"));
            return;
        }

        joystick = new wxJoystick(wxJOYSTICK1);

        if ( joystick == NULL )
        {
            CheckBoxEnJoystick->SetValue(false);
            wxMessageBox(_("Error opening Joystick 1"));
            Log->AppendText(_("Error opening Joystick 1\n"));
            return;
        }

        joyCenter = joystick->GetPosition(); // auto calibrate

        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.x), 0, 2);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.y), 1, 2);

        joyMax.x = joystick->GetXMax();
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMax.x), 0, 3);

        joyMax.y = joystick->GetYMax();
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMax.y), 1, 3);

        joyMin.x = joystick->GetXMin();
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMin.x), 0, 1);

        joyMin.y = joystick->GetYMin();
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyMin.y), 1, 1);

        Log->AppendText(_("\nJoystick information:\n"));

        Log->AppendText(_("ManufacturerId: ") + wxString::Format(wxT("%i"), joystick->GetManufacturerId())+_("\n"));
        Log->AppendText(_("ProductId: ") + wxString::Format(wxT("%i"), joystick->GetProductId())+_("\n"));
//        Log->AppendText(_("ProductName: ") + joystick->GetProductName()+_("\n"));
        Log->AppendText(_("MovementThreshold: ") + wxString::Format(wxT("%i"), joystick->GetMovementThreshold())+_("\n"));
        Log->AppendText(_("NumberAxes: ") + wxString::Format(wxT("%i"), joystick->GetNumberAxes())+_("\n"));
        Log->AppendText(_("NumberButtons: ") + wxString::Format(wxT("%i"), joystick->GetNumberButtons())+_("\n"));
        Log->AppendText(_("PollingMax: ") + wxString::Format(wxT("%i"), joystick->GetPollingMax())+_("\n"));
        Log->AppendText(_("PollingMin: ") + wxString::Format(wxT("%i"), joystick->GetPollingMin())+_("\n"));

        if(joystick->HasZ())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetZMin()), 2, 1);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetZPosition()), 2, 2);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetZMax()), 2, 3);
        }
        if(joystick->HasU())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetUMin()), 3, 1);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetUPosition()), 3, 2);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetUMax()), 3, 3);
        }
        if(joystick->HasV())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetVMin()), 4, 1);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetVPosition()), 4, 2);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetVMax()), 4, 3);
        }
        if(joystick->HasRudder())
        {
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetRudderMin()), 5, 1);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetRudderPosition()), 5, 2);
            GridJoy->SetCellValue (wxString::Format(wxT("%i"), joystick->GetRudderMax()), 5, 3);
        }
        if(joystick->HasPOV())
        {
            Log->AppendText(_("POVPosition:") + wxString::Format(wxT("%i"), joystick->GetPOVPosition())+_("\n"));
        }
        if(joystick->HasPOVCTS())
        {
            Log->AppendText(_("POVCTSPosition:") + wxString::Format(wxT("%i"), joystick->GetPOVCTSPosition())+_("\n"));
        }
        botoesAntes = joystick->GetButtonState();
    }
}

void serialcomFrame::OnButton9Click(wxCommandEvent& event)
{
    joyCenter = joyPos;

    GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.x), 0, 2);
    GridJoy->SetCellValue (wxString::Format(wxT("%i"), joyCenter.y), 1, 2);
}

void serialcomFrame::OnButton10Click(wxCommandEvent& event)
{
    Log->Clear();
}

void serialcomFrame::OnCheckBoxHandBrakeClick(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(VAR_FREIO, CheckBoxHandBrake->IsChecked() ? 1 : 0);
}

void serialcomFrame::OnButton12Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVar(VAR_T_POL);
    MbsBot::getInstance()->pedeVar(VAR_T_90);
    MbsBot::getInstance()->pedeVar(VAR_T_RF);
    MbsBot::getInstance()->pedeVar(VAR_PID);
}

void serialcomFrame::OnGridJoyCellLeftClick(wxGridEvent& event)
{
}

void serialcomFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
}
