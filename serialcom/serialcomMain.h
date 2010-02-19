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

#ifndef SERIALCOMMAIN_H
#define SERIALCOMMAIN_H

//(*Headers(serialcomFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include "MbsBot.h"

class serialcomFrame: public wxFrame
{
    public:

        serialcomFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~serialcomFrame();

    private:

		//(*Handlers(serialcomFrame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnSendCommandTextTextEnter(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnMenuItem3Selected(wxCommandEvent& event);
		void OnSlider1CmdSliderUpdated(wxScrollEvent& event);
		void OnSlider2CmdSliderUpdated(wxScrollEvent& event);
		void OnTextCtrl1TextEnter(wxCommandEvent& event);
		void OnTextCtrl2TextEnter(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnNotebook1PageChanged(wxNotebookEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton5Click(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void OnButton7Click(wxCommandEvent& event);
		void OnButton8Click(wxCommandEvent& event);
		//*)

        //(*Identifiers(serialcomFrame)
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
        static const long ID_SLIDER1;
        static const long ID_TEXTCTRL3;
        static const long ID_SLIDER2;
        static const long ID_TEXTCTRL4;
        static const long ID_SLIDER3;
        static const long ID_TEXTCTRL5;
        static const long ID_BUTTON2;
        static const long ID_BUTTON1;
        static const long ID_BUTTON3;
        static const long ID_PANEL2;
        static const long ID_BUTTON6;
        static const long ID_BUTTON7;
        static const long ID_BUTTON8;
        static const long ID_PANEL3;
        static const long ID_GRID1;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_PANEL4;
        static const long ID_NOTEBOOK1;
        static const long ID_TEXTCTRL1;
        static const long idMenuQuit;
        static const long ID_MENUITEM1;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(serialcomFrame)
        wxSlider* Slider1;
        wxTextCtrl* Log;
        wxButton* Button4;
        wxSlider* Slider2;
        wxNotebook* Notebook1;
        wxPanel* Panel4;
        wxMenu* Menu3;
        wxButton* Button1;
        wxSlider* Slider3;
        wxPanel* Panel1;
        wxGrid* Grid1;
        wxButton* Button2;
        wxPanel* Panel3;
        wxButton* Button6;
        wxButton* Button5;
        wxMenuItem* MenuItem3;
        wxButton* Button3;
        wxButton* Button7;
        wxStatusBar* StatusBar1;
        wxTextCtrl* TextCtrl2;
        wxTextCtrl* TextCtrl1;
        wxPanel* Panel2;
        wxTextCtrl* TextCtrl3;
        wxButton* Button8;
        wxTimer Timer1;
        wxTextCtrl* SendCommandText;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SERIALCOMMAIN_H
