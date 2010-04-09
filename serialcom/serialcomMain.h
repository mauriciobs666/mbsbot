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
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/stopwatch.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include "MbsBot.h"

class serialcomFrame: public wxFrame
{
    public:

		// Sensor data analysis stuff
		int counter;
		int accumulated[6][10];

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
		void OnSlider3CmdSliderUpdated(wxScrollEvent& event);
		void OnTextCtrl3TextEnter(wxCommandEvent& event);
		void OnButton11Click(wxCommandEvent& event);
		void OnButton14Click(wxCommandEvent& event);
		void OnButton15Click(wxCommandEvent& event);
		//*)

        //(*Identifiers(serialcomFrame)
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL6;
        static const long ID_STATICTEXT2;
        static const long ID_CHOICE1;
        static const long ID_BUTTON14;
        static const long ID_BUTTON11;
        static const long ID_PANEL6;
        static const long ID_BUTTON6;
        static const long ID_BUTTON7;
        static const long ID_BUTTON8;
        static const long ID_PANEL3;
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
        static const long ID_GRID1;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_PANEL4;
        static const long ID_BUTTON9;
        static const long ID_BUTTON10;
        static const long ID_TEXTCTRL7;
        static const long ID_BUTTON12;
        static const long ID_BUTTON13;
        static const long ID_PANEL5;
        static const long ID_NOTEBOOK1;
        static const long ID_TEXTCTRL1;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(serialcomFrame)
        wxSlider* Slider1;
        wxTextCtrl* TextCtrl4;
        wxPanel* Panel5;
        wxTextCtrl* Log;
        wxButton* Button4;
        wxSlider* Slider2;
        wxNotebook* Notebook1;
        wxStaticText* StaticText2;
        wxPanel* Panel4;
        wxButton* Button1;
        wxButton* Button14;
        wxSlider* Slider3;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxGrid* Grid1;
        wxButton* Button2;
        wxPanel* Panel6;
        wxPanel* Panel3;
        wxButton* Button6;
        wxButton* Button10;
        wxButton* Button11;
        wxButton* Button5;
        wxButton* Button3;
        wxButton* Button7;
        wxStopWatch StopWatch1;
        wxStatusBar* StatusBar1;
        wxButton* Button9;
        wxTextCtrl* TextCtrl2;
        wxTextCtrl* TextCtrl1;
        wxPanel* Panel2;
        wxTextCtrl* TextCtrl5;
        wxButton* Button13;
        wxTextCtrl* TextCtrl3;
        wxChoice* Choice1;
        wxButton* Button8;
        wxTimer Timer1;
        wxButton* Button12;
        wxTextCtrl* SendCommandText;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SERIALCOMMAIN_H
