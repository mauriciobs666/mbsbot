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

#include "Preferences.h"
#include "MbsBot.h"

//(*InternalHeaders(Preferences)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(Preferences)
const long Preferences::ID_STATICTEXT1 = wxNewId();
const long Preferences::ID_TEXTCTRL1 = wxNewId();
const long Preferences::ID_STATICTEXT2 = wxNewId();
const long Preferences::ID_CHOICE1 = wxNewId();
const long Preferences::ID_BUTTON1 = wxNewId();
const long Preferences::ID_BUTTON2 = wxNewId();
const long Preferences::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(Preferences,wxFrame)
	//(*EventTable(Preferences)
	//*)
END_EVENT_TABLE()

Preferences::Preferences(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(Preferences)
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(208,104));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(112,96), wxSize(168,72), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1 = new wxFlexGridSizer(1, 2, 0, 0);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Serial port"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("/dev/ttyUSB0"), wxDefaultPosition, wxSize(106,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(TextCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Speed"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	Choice1 = new wxChoice(Panel1, ID_CHOICE1, wxDefaultPosition, wxSize(106,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	Choice1->Append(_("9600"));
	Choice1->Append(_("19200"));
	Choice1->Append(_("38400"));
	Choice1->Append(_("57600"));
	Choice1->SetSelection( Choice1->Append(_("115200")) );
	FlexGridSizer1->Add(Choice1, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer1->Add(Button1, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer1->Add(Button2, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
	Panel1->SetSizer(FlexGridSizer1);
	FlexGridSizer1->SetSizeHints(Panel1);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Preferences::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Preferences::OnButton2Click);
	//*)
	int spd[] = { 9600, 19200, 38400, 57600, 115200 };
	int currSpd = MbsBot::getInstance()->getBaud();
	for(int x=0; x<sizeof(spd); x++)
		if( currSpd == spd[x])
		{
			Choice1->SetSelection(x);
		}

}

Preferences::~Preferences()
{
	//(*Destroy(Preferences)
	//*)
}


void Preferences::OnTextCtrl2Text(wxCommandEvent& event)
{
}

void Preferences::OnButton1Click(wxCommandEvent& event)
{
	int spd[] = { 9600, 19200, 38400, 57600, 115200 };
	MbsBot::getInstance()->init(TextCtrl1->GetValue().mb_str(wxConvUTF8), spd[Choice1->GetCurrentSelection()]);
	Close();
}

void Preferences::OnButton2Click(wxCommandEvent& event)
{
	Close();
}
