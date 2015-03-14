/**	Copyright (C) 2010-2013 - Mauricio Bieze Stefani
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
//#include "../version.h"

#include <wx/msgdlg.h>

//(*InternalHeaders(serialcomFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/font.h>
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
const long serialcomFrame::ID_BUTTON9 = wxNewId();
const long serialcomFrame::ID_GRID2 = wxNewId();
const long serialcomFrame::ID_PANEL6 = wxNewId();
const long serialcomFrame::ID_STATICTEXT6 = wxNewId();
const long serialcomFrame::ID_CHOICE3 = wxNewId();
const long serialcomFrame::ID_STATICTEXT7 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL9 = wxNewId();
const long serialcomFrame::ID_STATICTEXT8 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL10 = wxNewId();
const long serialcomFrame::ID_STATICTEXT4 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL15 = wxNewId();
const long serialcomFrame::ID_STATICTEXT5 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL16 = wxNewId();
const long serialcomFrame::ID_STATICTEXT9 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL11 = wxNewId();
const long serialcomFrame::ID_BUTTON6 = wxNewId();
const long serialcomFrame::ID_BUTTON7 = wxNewId();
const long serialcomFrame::ID_BUTTON8 = wxNewId();
const long serialcomFrame::ID_BUTTON12 = wxNewId();
const long serialcomFrame::ID_PANEL3 = wxNewId();
const long serialcomFrame::ID_STATICTEXT14 = wxNewId();
const long serialcomFrame::ID_STATICTEXT15 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL18 = wxNewId();
const long serialcomFrame::ID_STATICTEXT17 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL20 = wxNewId();
const long serialcomFrame::ID_STATICTEXT19 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL22 = wxNewId();
const long serialcomFrame::ID_STATICTEXT10 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL12 = wxNewId();
const long serialcomFrame::ID_STATICTEXT11 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL13 = wxNewId();
const long serialcomFrame::ID_STATICTEXT12 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL14 = wxNewId();
const long serialcomFrame::ID_BUTTON13 = wxNewId();
const long serialcomFrame::ID_BUTTON15 = wxNewId();
const long serialcomFrame::ID_BUTTON16 = wxNewId();
const long serialcomFrame::ID_PANEL7 = wxNewId();
const long serialcomFrame::ID_SLIDER1 = wxNewId();
const long serialcomFrame::ID_SLIDER2 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL3 = wxNewId();
const long serialcomFrame::ID_TEXTCTRL4 = wxNewId();
const long serialcomFrame::ID_CHECKBOX4 = wxNewId();
const long serialcomFrame::ID_CHECKBOX8 = wxNewId();
const long serialcomFrame::ID_BUTTON2 = wxNewId();
const long serialcomFrame::ID_BUTTON1 = wxNewId();
const long serialcomFrame::ID_BUTTON3 = wxNewId();
const long serialcomFrame::ID_CHOICE2 = wxNewId();
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
    wxStaticBoxSizer* StaticBoxSizer2;
    wxFlexGridSizer* FlexGridSizer4;
    wxStaticBoxSizer* StaticBoxSizer12;
    wxStaticBoxSizer* StaticBoxSizer15;
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
    wxFlexGridSizer* FlexGridSizer14;
    wxStaticBoxSizer* StaticBoxSizer11;
    wxBoxSizer* BoxSizer1;
    wxFlexGridSizer* FlexGridSizer12;
    wxFlexGridSizer* FlexGridSizer6;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer11;
    wxBoxSizer* BoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer5;

    Create(parent, wxID_ANY, _("MBSBOT - (c) 2010-2014 GPL - Mauricio Bieze Stefani"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    wxFont thisFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    if ( !thisFont.Ok() ) thisFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    SetFont(thisFont);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK2, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK2"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Log = new wxTextCtrl(Panel1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(500,275), wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer2->Add(Log, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    StaticBoxSizer9 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("Log"));
    FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
    CheckBoxRXdata = new wxCheckBox(Panel1, ID_CHECKBOX2, _("Dump RX"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxRXdata->SetValue(true);
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
    Button14 = new wxButton(Panel6, ID_BUTTON14, _("Desconectar"), wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_BUTTON14"));
    FlexGridSizer6->Add(Button14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button11 = new wxButton(Panel6, ID_BUTTON11, _("Conectar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    FlexGridSizer6->Add(Button11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer8->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer8, 1, wxALL|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer13 = new wxStaticBoxSizer(wxVERTICAL, Panel6, _("Gamepad"));
    FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer10 = new wxFlexGridSizer(0, 3, 0, 0);
    CheckBoxEnJoystick = new wxCheckBox(Panel6, ID_CHECKBOX3, _("Habilita"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    CheckBoxEnJoystick->SetValue(false);
    FlexGridSizer10->Add(CheckBoxEnJoystick, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel6, ID_BUTTON9, _("Centrar"), wxDefaultPosition, wxSize(76,27), 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer10->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(FlexGridSizer10, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    GridJoy = new wxGrid(Panel6, ID_GRID2, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GRID2"));
    GridJoy->CreateGrid(7,4);
    GridJoy->EnableEditing(false);
    GridJoy->EnableGridLines(true);
    GridJoy->SetColLabelValue(0, _("Local"));
    GridJoy->SetColLabelValue(1, _("Min"));
    GridJoy->SetColLabelValue(2, _("Centro"));
    GridJoy->SetColLabelValue(3, _("Max"));
    GridJoy->SetRowLabelValue(0, _("X"));
    GridJoy->SetRowLabelValue(1, _("Y"));
    GridJoy->SetRowLabelValue(2, _("Z"));
    GridJoy->SetRowLabelValue(3, _("Rudder"));
    GridJoy->SetRowLabelValue(4, _("U"));
    GridJoy->SetRowLabelValue(5, _("V"));
    GridJoy->SetRowLabelValue(6, _("Botoes"));
    GridJoy->SetDefaultCellFont( GridJoy->GetFont() );
    GridJoy->SetDefaultCellTextColour( GridJoy->GetForegroundColour() );
    FlexGridSizer7->Add(GridJoy, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer13->Add(FlexGridSizer7, 1, wxALL|wxEXPAND|wxSHAPED|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer5->Add(StaticBoxSizer13, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel6->SetSizer(FlexGridSizer5);
    FlexGridSizer5->Fit(Panel6);
    FlexGridSizer5->SetSizeHints(Panel6);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer12 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Variaveis"));
    FlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText5 = new wxStaticText(Panel3, ID_STATICTEXT6, _("Programa (p):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer12->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoicePrg = new wxChoice(Panel3, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
    ChoicePrg->Append(_("PRG_RC"));
    ChoicePrg->Append(_("PRG_RC_SERIAL"));
    ChoicePrg->Append(_("PRG_FOTOVORO"));
    ChoicePrg->Append(_("PRG_LINE_FOLLOWER"));
    ChoicePrg->Append(_("PRG_SCANNER"));
    ChoicePrg->Append(_("PRG_CHASE"));
    ChoicePrg->Append(_("PRG_COLISAO"));
    ChoicePrg->Append(_("PRG_SENTINELA"));
    ChoicePrg->Append(_("PRG_WIICHUCK"));
    ChoicePrg->Append(_("PRG_SCOPE"));
    ChoicePrg->Append(_("PRG_KNOB"));
    ChoicePrg->Append(_("PRG_NAV_3S"));
    FlexGridSizer12->Add(ChoicePrg, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel3, ID_STATICTEXT7, _("Vel Max % (vm):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer12->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlVelMax = new wxTextCtrl(Panel3, ID_TEXTCTRL9, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL9"));
    FlexGridSizer12->Add(TextCtrlVelMax, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel3, ID_STATICTEXT8, _("Vel Escala % (ve)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer12->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlVelEscala = new wxTextCtrl(Panel3, ID_TEXTCTRL10, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL10"));
    FlexGridSizer12->Add(TextCtrlVelEscala, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(Panel3, ID_STATICTEXT4, _("Balanco -esq / +dir (bal):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer12->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlBalanco = new wxTextCtrl(Panel3, ID_TEXTCTRL15, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL15"));
    FlexGridSizer12->Add(TextCtrlBalanco, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText12 = new wxStaticText(Panel3, ID_STATICTEXT5, _("Delay motores (dm):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer12->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayMotor = new wxTextCtrl(Panel3, ID_TEXTCTRL16, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL16"));
    FlexGridSizer12->Add(TextCtrlDelayMotor, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(Panel3, ID_STATICTEXT9, _("Delay E/S (drf):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer12->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayRead = new wxTextCtrl(Panel3, ID_TEXTCTRL11, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL11"));
    FlexGridSizer12->Add(TextCtrlDelayRead, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer12->Add(FlexGridSizer12, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer12, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Controles"));
    Button6 = new wxButton(Panel3, ID_BUTTON6, _("Salvar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    StaticBoxSizer6->Add(Button6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel3, ID_BUTTON7, _("Desfazer"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    StaticBoxSizer6->Add(Button7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel3, ID_BUTTON8, _("Default"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    StaticBoxSizer6->Add(Button8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button12 = new wxButton(Panel3, ID_BUTTON12, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON12"));
    StaticBoxSizer6->Add(Button12, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer6, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel3->SetSizer(FlexGridSizer3);
    FlexGridSizer3->Fit(Panel3);
    FlexGridSizer3->SetSizeHints(Panel3);
    Panel7 = new wxPanel(Notebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    FlexGridSizer9 = new wxFlexGridSizer(0, 3, 0, 0);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel7, _("Line Follower"));
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticText14 = new wxStaticText(Panel7, ID_STATICTEXT14, _("erro = entrada - setPoint;\nP = erro * Kp;\nI = constrain( I += erro * Ki * dT, minMV, maxMV );\nD = ( dEntrada \? dEntrada : dErro ) * Kd / dT;\nMV = constrain( P + I + D, minMV, maxMV );"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    BoxSizer3->Add(StaticText14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer14 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText15 = new wxStaticText(Panel7, ID_STATICTEXT15, _("kp"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    FlexGridSizer14->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKp = new wxTextCtrl(Panel7, ID_TEXTCTRL18, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL18"));
    FlexGridSizer14->Add(TextCtrlKp, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText17 = new wxStaticText(Panel7, ID_STATICTEXT17, _("ki"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
    FlexGridSizer14->Add(StaticText17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKi = new wxTextCtrl(Panel7, ID_TEXTCTRL20, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL20"));
    FlexGridSizer14->Add(TextCtrlKi, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText19 = new wxStaticText(Panel7, ID_STATICTEXT19, _("kd"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
    FlexGridSizer14->Add(StaticText19, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlKd = new wxTextCtrl(Panel7, ID_TEXTCTRL22, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL22"));
    FlexGridSizer14->Add(TextCtrlKd, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(Panel7, ID_STATICTEXT10, _("Max MV (mmv)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer14->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlMmv = new wxTextCtrl(Panel7, ID_TEXTCTRL12, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL12"));
    FlexGridSizer14->Add(TextCtrlMmv, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(Panel7, ID_STATICTEXT11, _("Debounce (deb)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer14->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDelayDeb = new wxTextCtrl(Panel7, ID_TEXTCTRL13, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL13"));
    FlexGridSizer14->Add(TextCtrlDelayDeb, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText11 = new wxStaticText(Panel7, ID_STATICTEXT12, _("Zera acumulador (zac)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer14->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlZac = new wxTextCtrl(Panel7, ID_TEXTCTRL14, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL14"));
    FlexGridSizer14->Add(TextCtrlZac, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(FlexGridSizer14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer9->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    StaticBoxSizer15 = new wxStaticBoxSizer(wxVERTICAL, Panel7, _("Controles"));
    Button13 = new wxButton(Panel7, ID_BUTTON13, _("Salvar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON13"));
    StaticBoxSizer15->Add(Button13, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button15 = new wxButton(Panel7, ID_BUTTON15, _("Desfazer"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON15"));
    StaticBoxSizer15->Add(Button15, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button16 = new wxButton(Panel7, ID_BUTTON16, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON16"));
    StaticBoxSizer15->Add(Button16, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer9->Add(StaticBoxSizer15, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel7->SetSizer(FlexGridSizer9);
    FlexGridSizer9->Fit(Panel7);
    FlexGridSizer9->SetSizeHints(Panel7);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FlexGridSizer1 = new wxFlexGridSizer(1, 6, 0, 0);
    StaticBoxSizer14 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Rodas"));
    FlexGridSizer11 = new wxFlexGridSizer(2, 2, 0, 0);
    SliderRodaEsquerda = new wxSlider(Panel2, ID_SLIDER1, 0, -255, 255, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER1"));
    FlexGridSizer11->Add(SliderRodaEsquerda, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SliderRodaDireita = new wxSlider(Panel2, ID_SLIDER2, 0, -255, 255, wxDefaultPosition, wxSize(50,150), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER2"));
    FlexGridSizer11->Add(SliderRodaDireita, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlRodaEsquerda = new wxTextCtrl(Panel2, ID_TEXTCTRL3, _("0"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer11->Add(TextCtrlRodaEsquerda, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlRodaDireita = new wxTextCtrl(Panel2, ID_TEXTCTRL4, _("0"), wxDefaultPosition, wxSize(50,25), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer11->Add(TextCtrlRodaDireita, 0, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer14->Add(FlexGridSizer11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer14, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Controles"));
    CheckBoxAutoRefresh = new wxCheckBox(Panel2, ID_CHECKBOX4, _("Refresh"), wxDefaultPosition, wxSize(88,33), 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    CheckBoxAutoRefresh->SetValue(false);
    StaticBoxSizer3->Add(CheckBoxAutoRefresh, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxPoll = new wxCheckBox(Panel2, ID_CHECKBOX8, _("Polling"), wxDefaultPosition, wxSize(89,34), 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    CheckBoxPoll->SetValue(false);
    StaticBoxSizer3->Add(CheckBoxPoll, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(Panel2, ID_BUTTON2, _("Refresh"), wxDefaultPosition, wxSize(110,27), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticBoxSizer3->Add(Button2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel2, ID_BUTTON1, _("Salvar centro"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer3->Add(Button1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(Panel2, ID_BUTTON3, _("Parar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer3->Add(Button3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice2 = new wxChoice(Panel2, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    Choice2->Append(_("Servo"));
    Choice2->SetSelection( Choice2->Append(_("PWM")) );
    StaticBoxSizer3->Add(Choice2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
    Grid1 = new wxGrid(Panel4, ID_GRID1, wxDefaultPosition, wxSize(345,455), 0, _T("ID_GRID1"));
    Grid1->CreateGrid(16,3);
    Grid1->EnableEditing(true);
    Grid1->EnableGridLines(true);
    Grid1->SetColLabelValue(0, _("Agora"));
    Grid1->SetColLabelValue(1, _("Min"));
    Grid1->SetColLabelValue(2, _("Max"));
    Grid1->SetRowLabelValue(0, _("0"));
    Grid1->SetRowLabelValue(1, _("1"));
    Grid1->SetRowLabelValue(2, _("2"));
    Grid1->SetRowLabelValue(3, _("3"));
    Grid1->SetRowLabelValue(4, _("4"));
    Grid1->SetRowLabelValue(5, _("5"));
    Grid1->SetRowLabelValue(6, _("6"));
    Grid1->SetRowLabelValue(7, _("7"));
    Grid1->SetRowLabelValue(8, _("8"));
    Grid1->SetRowLabelValue(9, _("9"));
    Grid1->SetRowLabelValue(10, _("10"));
    Grid1->SetRowLabelValue(11, _("11"));
    Grid1->SetRowLabelValue(12, _("12"));
    Grid1->SetRowLabelValue(13, _("13"));
    Grid1->SetRowLabelValue(14, _("14"));
    Grid1->SetRowLabelValue(15, _("15"));
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
    Grid1->SetCellValue(6, 0, _("0"));
    Grid1->SetCellValue(6, 1, _("1023"));
    Grid1->SetCellValue(6, 2, _("0"));
    Grid1->SetCellValue(7, 0, _("0"));
    Grid1->SetCellValue(7, 1, _("1023"));
    Grid1->SetCellValue(7, 2, _("0"));
    Grid1->SetCellValue(8, 0, _("0"));
    Grid1->SetCellValue(8, 1, _("1023"));
    Grid1->SetCellValue(8, 2, _("0"));
    Grid1->SetCellValue(9, 0, _("0"));
    Grid1->SetCellValue(9, 1, _("1023"));
    Grid1->SetCellValue(9, 2, _("0"));
    Grid1->SetCellValue(10, 0, _("0"));
    Grid1->SetCellValue(10, 1, _("1023"));
    Grid1->SetCellValue(10, 2, _("0"));
    Grid1->SetCellValue(11, 0, _("0"));
    Grid1->SetCellValue(11, 1, _("1023"));
    Grid1->SetCellValue(11, 2, _("0"));
    Grid1->SetCellValue(12, 0, _("0"));
    Grid1->SetCellValue(12, 1, _("1023"));
    Grid1->SetCellValue(12, 2, _("0"));
    Grid1->SetCellValue(13, 0, _("0"));
    Grid1->SetCellValue(13, 1, _("1023"));
    Grid1->SetCellValue(13, 2, _("0"));
    Grid1->SetCellValue(14, 0, _("0"));
    Grid1->SetCellValue(14, 1, _("1023"));
    Grid1->SetCellValue(14, 2, _("0"));
    Grid1->SetCellValue(15, 0, _("0"));
    Grid1->SetCellValue(15, 1, _("1023"));
    Grid1->SetCellValue(15, 2, _("0"));
    Grid1->SetDefaultCellFont( Grid1->GetFont() );
    Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );
    StaticBoxSizer4->Add(Grid1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxVERTICAL, Panel4, _("Controles"));
    CheckBoxAutoRefreshSensors = new wxCheckBox(Panel4, ID_CHECKBOX7, _("Polling"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    CheckBoxAutoRefreshSensors->SetValue(false);
    StaticBoxSizer5->Add(CheckBoxAutoRefreshSensors, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(Panel4, ID_BUTTON4, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    StaticBoxSizer5->Add(Button4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(Panel4, ID_BUTTON5, _("Limpar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    StaticBoxSizer5->Add(Button5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(StaticBoxSizer5, 0, wxALL|wxSHAPED|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer2->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel4->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel4);
    FlexGridSizer2->SetSizeHints(Panel4);
    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    FlexGridSizer4 = new wxFlexGridSizer(3, 1, 0, 0);
    StaticBitmap1 = new wxStaticBitmap(Panel5, ID_STATICBITMAP1, wxBitmap(wxImage(_T("m2d2.jpg"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    FlexGridSizer4->Add(StaticBitmap1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel5, ID_STATICTEXT3, _("\nCopyright (C) 2010-2014 - Mauricio Bieze Stefani\n\nMBSBOT is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nMBSBOT is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.\n"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer4->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    Panel5->SetSizer(FlexGridSizer4);
    FlexGridSizer4->Fit(Panel5);
    FlexGridSizer4->SetSizeHints(Panel5);
    Notebook1->AddPage(Panel1, _("Log"), false);
    Notebook1->AddPage(Panel6, _("Local"), false);
    Notebook1->AddPage(Panel3, _("EEPROM"), false);
    Notebook1->AddPage(Panel7, _("PID"), false);
    Notebook1->AddPage(Panel2, _("Motores"), false);
    Notebook1->AddPage(Panel4, _("Sensores"), false);
    Notebook1->AddPage(Panel5, _("GPL"), false);
    BoxSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SendCommandText = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(SendCommandText, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[3] = { -10, -10, 70 };
    int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(3,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(3,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(100, false);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(ID_TOOLBAR_DEFAULT, _("Novo"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Carrega valores default"), wxEmptyString);
    ToolBarItem2 = ToolBar1->AddTool(ID_TOOLBAR_SALVAR, _("Salvar"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Salva dados na EEPROM"), wxEmptyString);
    ToolBarItem3 = ToolBar1->AddTool(ID_TOOLBAR_CARREGAR, _("Carregar"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Carrega config da EEPROM"), wxEmptyString);
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton10Click);
    Connect(ID_TEXTCTRL6,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnButton11Click);
    Connect(ID_BUTTON14,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton14Click);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton11Click);
    Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnCheckBoxJoystick);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton9Click);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnChoiceProgram);
    Connect(ID_TEXTCTRL9,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlVelMaxTextEnter);
    Connect(ID_TEXTCTRL10,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlVelEscalaTurnTextEnter);
    Connect(ID_TEXTCTRL15,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlBalancoTextEnter);
    Connect(ID_TEXTCTRL16,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlDelayMotorTextEnter);
    Connect(ID_TEXTCTRL11,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlDelayReadTextEnter);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton6Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton7Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton8Click);
    Connect(ID_BUTTON12,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton12Click);
    Connect(ID_TEXTCTRL18,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlKpTextEnter);
    Connect(ID_TEXTCTRL20,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlKiTextEnter);
    Connect(ID_TEXTCTRL22,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlKdTextEnter);
    Connect(ID_TEXTCTRL12,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlMmvTextEnter);
    Connect(ID_TEXTCTRL13,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlDelayDebTextEnter);
    Connect(ID_TEXTCTRL14,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlZacTextEnter);
    Connect(ID_BUTTON13,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton6Click);
    Connect(ID_BUTTON15,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton7Click);
    Connect(ID_BUTTON16,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton12Click);
    Connect(ID_SLIDER1,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider1CmdSliderUpdated);
    Connect(ID_SLIDER2,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider2CmdSliderUpdated);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl1TextEnter);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl2TextEnter);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton2Click);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton1Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton3Click);
    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&serialcomFrame::OnChoiceDCServo);
    Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnCheckBoxHandBrakeClick);
    Connect(ID_SLIDER4,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSliderTiltCmdSliderUpdated);
    Connect(ID_TEXTCTRL7,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlTiltTextEnter);
    Connect(ID_SLIDER3,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSlider3CmdSliderUpdated);
    Connect(ID_TEXTCTRL5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrl3TextEnter);
    Connect(ID_SLIDER5,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&serialcomFrame::OnSliderRollCmdSliderUpdated);
    Connect(ID_TEXTCTRL8,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnTextCtrlRollTextEnter);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton4Click);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&serialcomFrame::OnButton5Click);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&serialcomFrame::OnSendCommandTextTextEnter);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&serialcomFrame::OnTimer1Trigger);
    //*)

    if( MbsBot::getInstance()->init() == 0)
    {
        SetStatusText(_("Conectado"));
        MbsBot::getInstance()->pedeVars();
    }
    else
        SetStatusText(_("Erro na porta serial"));

    int currSpd = MbsBot::getInstance()->getBaud();
    for(int x=0; x < n_spd; x++)
        if( currSpd == spd[x])
            Choice1->SetSelection(x);
    TextCtrl4->SetValue(wxString(MbsBot::getInstance()->getPorta(), wxConvUTF8));

    if ( wxJoystick::GetNumberJoysticks() > 0 )
    {
        // abre joystick
        if ( (joystick = new wxJoystick(wxJOYSTICK1)) )
            CheckBoxEnJoystick->SetValue(true);
    }
    else
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
//      Dispositivos locais
// ============================================================================

void serialcomFrame::OnButton11Click(wxCommandEvent& event)
{
    if(MbsBot::getInstance()->init(TextCtrl4->GetValue().mb_str(wxConvUTF8), spd[Choice1->GetCurrentSelection()]) == 0)
    {
        SetStatusText(_("Conectado"));
        MbsBot::getInstance()->pedeVars();
    }
    else
        SetStatusText(_("Erro na porta serial"));
}
void serialcomFrame::OnButton14Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->desconecta();
    SetStatusText(_("Desconectado"));
}

// ============================================================================
//      EEPROM
// ============================================================================

void serialcomFrame::OnButton6Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->salva();
    MbsBot::getInstance()->pedeVars();
}
void serialcomFrame::OnButton7Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->carrega();
    MbsBot::getInstance()->pedeVars();
}
void serialcomFrame::OnButton8Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->carregaDefaults();
    MbsBot::getInstance()->pedeVars();
}
void serialcomFrame::OnButton12Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVars();
}

// ============================================================================
//      MOTORES
// ============================================================================

// controles da roda esquerda

void serialcomFrame::OnSlider1CmdSliderUpdated(wxScrollEvent& event)
{
    int val = SliderRodaEsquerda->GetValue();
//    TextCtrlRodaEsquerda->SetValue(wxString::Format(wxT("%i"), val));
    MbsBot::getInstance()->setRodaEsquerda(val);
}
void serialcomFrame::OnTextCtrl1TextEnter(wxCommandEvent& event)
{
    int val = atoi(TextCtrlRodaEsquerda->GetValue().mb_str(wxConvUTF8));
//    SliderRodaEsquerda->SetValue(val);
    MbsBot::getInstance()->setRodaEsquerda(val);
}

// controles da roda direita

void serialcomFrame::OnSlider2CmdSliderUpdated(wxScrollEvent& event)
{
    int val = SliderRodaDireita->GetValue();
//    TextCtrlRodaDireita->SetValue(wxString::Format(wxT("%i"), val));
    MbsBot::getInstance()->setRodaDireita(val);
}
void serialcomFrame::OnTextCtrl2TextEnter(wxCommandEvent& event)
{
    int val = atoi(TextCtrlRodaDireita->GetValue().mb_str(wxConvUTF8));
//    SliderRodaDireita->SetValue(val);
    MbsBot::getInstance()->setRodaDireita(val);
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

// Botao salvar centro

void serialcomFrame::OnButton1Click(wxCommandEvent& event)
{
    if(wxYES == wxMessageBox(_("Certeza?"), _("Sobrescrever centro?"), wxYES_NO))
    {
        MbsBot::getInstance()->setCentroRodaEsquerda(SliderRodaEsquerda->GetValue());
        MbsBot::getInstance()->setCentroRodaDireita(SliderRodaDireita->GetValue());
    }
}

// Botao Refresh

void serialcomFrame::OnButton2Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVars();
}

// Botao Parar

void serialcomFrame::OnButton3Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->stop();
    MbsBot::getInstance()->pedeVars();
}

// ============================================================================
//      SENSORES
// ============================================================================

void serialcomFrame::OnButton4Click(wxCommandEvent& event)
{
    MbsBot::getInstance()->pedeVar(NOME_AS);  // todas entradas analogicas An
}

void serialcomFrame::OnButton5Click(wxCommandEvent& event)
{
    for(int s = 0; s < 16; s++)
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
    // tempo do timer = 100ms

    MbsBot *mbsbot = MbsBot::getInstance();

    // executa coisas uma vez por segundo
    static char conta1s = 10;
    if( --conta1s == 0)
    {
        if(CheckBoxPoll->IsChecked())
            mbsbot->status();
        if(CheckBoxAutoRefreshSensors->IsChecked())
            mbsbot->pedeVar(NOME_AS);
        conta1s = 10;
    }

    char *rx;
    while ((rx = mbsbot->recebe()))
    {
        wxString str = wxString(rx,wxConvUTF8);

        if(CheckBoxRXdata->IsChecked())
            Log->AppendText(str);

        if(*rx == 's')	// status
        {
            SetStatusText(str,1); // gambeta feia
        }

        // aba "Motores"
        if(CheckBoxAutoRefresh->IsChecked())
        {
            SliderRodaEsquerda->SetValue(mbsbot->getRodaEsquerda());
            TextCtrlRodaEsquerda->SetValue(wxString::Format(wxT("%i"), mbsbot->getRodaEsquerda()));

            SliderRodaDireita->SetValue(mbsbot->getRodaDireita());
            TextCtrlRodaDireita->SetValue(wxString::Format(wxT("%i"), mbsbot->getRodaDireita()));

            SliderPan->SetValue(mbsbot->getServoPan());
            TextCtrlPan->SetValue(wxString::Format(wxT("%i"), mbsbot->getServoPan()));

            SliderTilt->SetValue(mbsbot->getServoTilt());
            TextCtrlTilt->SetValue(wxString::Format(wxT("%i"), mbsbot->getServoTilt()));

            SliderRoll->SetValue(mbsbot->getServoRoll());
            TextCtrlRoll->SetValue(wxString::Format(wxT("%i"), mbsbot->getServoRoll()));

        }
        CheckBoxHandBrake->SetValue(mbsbot->getFreioMao());

        // aba "EEPROM"
        ChoicePrg->SetSelection(mbsbot->getPrograma());
        TextCtrlVelMax->SetValue(wxString::Format(wxT("%i"), mbsbot->getVelMax()));
        TextCtrlVelEscala->SetValue(wxString::Format(wxT("%i"), mbsbot->getVelEscala()));
        TextCtrlBalanco->SetValue(wxString::Format(wxT("%i"), mbsbot->getBalanco()));
        TextCtrlDelayMotor->SetValue(wxString::Format(wxT("%i"), mbsbot->getDelayM()));
        TextCtrlDelayRead->SetValue(wxString::Format(wxT("%i"), mbsbot->getDelayRF()));

        // aba PID
        TextCtrlDelayDeb->SetValue(wxString::Format(wxT("%i"), mbsbot->getDelayDeb()));
        TextCtrlKp->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidKP()));
        TextCtrlKi->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidKI()));
        TextCtrlKd->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidKD()));
//        TextCtrlLmp->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidLmp()));
//        TextCtrlLmi->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidLmi()));
//        TextCtrlLmd->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidLmd()));
//        TextCtrlMmv->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidMmv()));
        TextCtrlZac->SetValue(wxString::Format(wxT("%i"), mbsbot->getPidZac()));

        // aba Sensores
        for(int s = 0; s < 16; s++)
        {
            int valor = mbsbot->getSensor(s);

            // leitura atual
            wxString str=wxString::Format(wxT("%i"), valor);
            Grid1->SetCellValue (str, s, 0);

            // valor minimo
            if(valor < atoi(Grid1->GetCellValue (s, 1).mb_str(wxConvUTF8)))
                Grid1->SetCellValue (str, s, 1);

            // valor maximo
            if(valor > atoi(Grid1->GetCellValue (s, 2).mb_str(wxConvUTF8)))
                Grid1->SetCellValue (str, s, 2);
/*
            // media das 10 ultimas
            sensorAvg[s].read(valor);
            str = wxString::Format(wxT("%i"), sensorAvg[s].getAverage());
            Grid1->SetCellValue (str, s, 3);
*/
        }
    }

    // verifica erros no joystick
    if ( joystick && !joystick->IsOk())
    {
        delete joystick;
        joystick = NULL;
        CheckBoxEnJoystick->SetValue(false);
    }

    if( joystick )
    {
        // le valores atuais
        // onde:
        //      x < centro = esquerda
        //      x > centro = direita
        //      y < centro = p/ cima
        //      y > centro = p/ baixo

        // X e Y
        wxPoint wxpt = joystick->GetPosition();

        int z = 0;
        int r = 0;
        int u = 0;
        int v = 0;

        // X2 (Z) e Y2 (Rudder)
        if(joystick->HasZ())        z = joystick->GetZPosition();
        if(joystick->HasRudder())   r = joystick->GetRudderPosition();

        // nao usados ainda
        if(joystick->HasU())    u = joystick->GetUPosition();
        if(joystick->HasV())    v = joystick->GetVPosition();

        unsigned short bt = joystick->GetButtonState();

        // Log
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), wxpt.x),  0, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), wxpt.y),  1, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), z),  2, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), r),  3, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), u),  4, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), v),  5, 0);
        GridJoy->SetCellValue (wxString::Format(wxT("%i"), bt), 6, 0);

        // Envia posicoes joystick
        mbsbot->enviaJoystick( bt, wxpt.x, wxpt.y, z, r );
    }
}

void serialcomFrame::OnChoiceDCServo(wxCommandEvent& event)
{
    if( Choice2->GetCurrentSelection() == 0 )
    {
        // Limites Servo
        SliderRodaEsquerda->SetRange(1000,2000);
        SliderRodaDireita->SetRange(1000,2000);
    }
    else if ( Choice2->GetCurrentSelection() == 1 )
    {
        // Limites PWM
        SliderRodaEsquerda->SetRange(-255,255);
        SliderRodaDireita->SetRange(-255,255);
    }
    MbsBot::getInstance()->pedeVar(NOME_RODA_ESQ);
    MbsBot::getInstance()->pedeVar(NOME_RODA_DIR);
}

void serialcomFrame::OnChoiceProgram(wxCommandEvent& event)
{
    MbsBot::getInstance()->setPrograma((enum Programas)(ChoicePrg->GetCurrentSelection()));
}

void serialcomFrame::OnCheckBoxJoystick(wxCommandEvent& event)
{
    // limpa tudo

    if ( joystick != NULL)
    {
        delete joystick;
        joystick = NULL;
    }

    if(CheckBoxEnJoystick->IsChecked())
    {
        if ( wxJoystick::GetNumberJoysticks() <= 0 )
        {
            CheckBoxEnJoystick->SetValue(false);
            wxMessageBox(_("Nenhum joystick disponivel"));
            Log->AppendText(_("Nenhum joystick disponivel\n"));
            return;
        }

        // abre joystick

        if ( ! (joystick = new wxJoystick(wxJOYSTICK1)) )
        {
            CheckBoxEnJoystick->SetValue(false);
            wxMessageBox(_("Erro ao abrir Joystick 1"));
            Log->AppendText(_("Erro ao abrir Joystick 1\n"));
            return;
        }

        Log->AppendText(_("\nInformacoes sobre o joystick:\n"));

        Log->AppendText(_("ManufacturerId: ") + wxString::Format(wxT("%i"), joystick->GetManufacturerId())+_("\n"));
        Log->AppendText(_("ProductId: ") + wxString::Format(wxT("%i"), joystick->GetProductId())+_("\n"));
        /*
        Can't open registry key 'HKLM\System\CurrentControlSet\Control\MediaResources\Joystick\DINPUT.DLL\CurrentJoystickSettings' (error 2: o sistema não pode encontrar o arquivo especificado.)
        Can't read value of 'HKLM\System\CurrentControlSet\Control\MediaResources\Joystick\DINPUT.DLL\CurrentJoystickSettings\Joystick1OEMName' (error 2: o sistema não pode encontrar o arquivo especificado.)
        Can't read value of 'HKLM\System\CurrentControlSet\Control\MediaProperties\PrivateProperties\Joystick\OEM\OEMName' (error 2: o sistema não pode encontrar o arquivo especificado.)
        Log->AppendText(_("ProductName: ") + joystick->GetProductName()+_("\n"));
        */
        Log->AppendText(_("MovementThreshold: ") + wxString::Format(wxT("%i"), joystick->GetMovementThreshold())+_("\n"));
        Log->AppendText(_("NumberAxes: ") + wxString::Format(wxT("%i"), joystick->GetNumberAxes())+_("\n"));
        Log->AppendText(_("NumberButtons: ") + wxString::Format(wxT("%i"), joystick->GetNumberButtons())+_("\n"));
        Log->AppendText(_("PollingMax: ") + wxString::Format(wxT("%i"), joystick->GetPollingMax())+_("\n"));
        Log->AppendText(_("PollingMin: ") + wxString::Format(wxT("%i"), joystick->GetPollingMin())+_("\n"));
        Log->AppendText(_("X min: ") + wxString::Format(wxT("%i"), joystick->GetXMin()));
        Log->AppendText(_(" max: ") + wxString::Format(wxT("%i"), joystick->GetXMax()));
        Log->AppendText(_(" centro: ") + wxString::Format(wxT("%i"), joystick->GetXMax())+_("\n"));
        Log->AppendText(_("Y min: ") + wxString::Format(wxT("%i"), joystick->GetYMin()));
        Log->AppendText(_(" max: ") + wxString::Format(wxT("%i"), joystick->GetYMax()));
        Log->AppendText(_(" centro: ") + wxString::Format(wxT("%i"), joystick->GetYMax())+_("\n"));

        if(joystick->HasPOV())
        {
            Log->AppendText(_("POVPosition:") + wxString::Format(wxT("%i"), joystick->GetPOVPosition())+_("\n"));
        }
        if(joystick->HasPOVCTS())
        {
            Log->AppendText(_("POVCTSPosition:") + wxString::Format(wxT("%i"), joystick->GetPOVCTSPosition())+_("\n"));
        }
    }
}

void serialcomFrame::OnButton9Click(wxCommandEvent& event)
{
// TODO (mbs#1#): Mandar comando pra centrar joystick
//    mbsJoystick.centrar();
}

void serialcomFrame::OnButton10Click(wxCommandEvent& event)
{
    Log->Clear();
}

void serialcomFrame::OnCheckBoxHandBrakeClick(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_FREIO, CheckBoxHandBrake->IsChecked() ? 1 : 0);
}

void serialcomFrame::OnTextCtrlVelMaxTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_VEL_MAX, atoi(TextCtrlVelMax->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlVelEscalaTurnTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_VEL_ESCALA, atoi(TextCtrlVelEscala->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlBalancoTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_BALANCO, atoi(TextCtrlBalanco->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlDelayMotorTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_T_MOTOR, atoi(TextCtrlDelayMotor->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlDelayReadTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_T_RF, atoi(TextCtrlDelayRead->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlKpTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_PID_KP, atoi(TextCtrlKp->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlKiTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_PID_KI, atoi(TextCtrlKi->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlKdTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_PID_KD, atoi(TextCtrlKd->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlLmpTextEnter(wxCommandEvent& event)
{
    //MbsBot::getInstance()->enviaVar(NOME_PID_LIM_P, atoi(TextCtrlLmp->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlLmiTextEnter(wxCommandEvent& event)
{
    //MbsBot::getInstance()->enviaVar(NOME_PID_LIM_I, atoi(TextCtrlLmi->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlLmdTextEnter(wxCommandEvent& event)
{
    //MbsBot::getInstance()->enviaVar(NOME_PID_LIM_D, atoi(TextCtrlLmd->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlMmvTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_PID_MVX, atoi(TextCtrlMmv->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlDelayDebTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_T_DEB, atoi(TextCtrlDelayDeb->GetValue().mb_str(wxConvUTF8)));
}

void serialcomFrame::OnTextCtrlZacTextEnter(wxCommandEvent& event)
{
    MbsBot::getInstance()->enviaVar(NOME_PID_ZAC, atoi(TextCtrlZac->GetValue().mb_str(wxConvUTF8)));
}
