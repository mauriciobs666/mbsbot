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
#include <wx/checkbox.h>
#include <wx/toolbar.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/choice.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include "MbsBot.h"
#include "Average.hpp"
#include <wx/joystick.h>

class serialcomFrame: public wxFrame
{
public:

    // Sensor data analysis stuff
    Average sensorAvg[6];

    // Joystick
    wxJoystick *joystick;
    wxPoint joyPos;
    wxPoint joyMax;
    wxPoint joyMin;
    wxPoint joyCenter;
    int botoesAntes;

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
    void OnDriveByKeyboard(wxCommandEvent& event);
    void OnButton15Click1(wxCommandEvent& event);
    void OnDriveTimerTrigger(wxTimerEvent& event);
    void OnChoiceDCServo(wxCommandEvent& event);
    void OnChoiceProgram(wxCommandEvent& event);
    void OnCheckBoxJoystick(wxCommandEvent& event);
    void OnButton9Click(wxCommandEvent& event);
    void OnButton10Click(wxCommandEvent& event);
    void OnSliderTiltCmdSliderUpdated(wxScrollEvent& event);
    void OnTextCtrlTiltTextEnter(wxCommandEvent& event);
    void OnSliderRollCmdSliderUpdated(wxScrollEvent& event);
    void OnTextCtrlRollTextEnter(wxCommandEvent& event);
    void OnTextCtrlRollText(wxCommandEvent& event);
    void OnCheckBoxHandBrakeClick(wxCommandEvent& event);
    void OnLogText(wxCommandEvent& event);
    void OnButton12Click(wxCommandEvent& event);
    void OnGridJoyCellLeftClick(wxGridEvent& event);
    void OnNotebook1PageChanged(wxNotebookEvent& event);
    //*)

    //(*Identifiers(serialcomFrame)
    static const long ID_TEXTCTRL2;
    static const long ID_CHECKBOX2;
    static const long ID_BUTTON10;
    static const long ID_PANEL1;
    static const long ID_STATICTEXT1;
    static const long ID_TEXTCTRL6;
    static const long ID_STATICTEXT2;
    static const long ID_CHOICE1;
    static const long ID_BUTTON14;
    static const long ID_BUTTON11;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX1;
    static const long ID_CHECKBOX6;
    static const long ID_GRID2;
    static const long ID_BUTTON9;
    static const long ID_STATICTEXT5;
    static const long ID_STATICTEXT4;
    static const long ID_PANEL6;
    static const long ID_STATICTEXT6;
    static const long ID_CHOICE3;
    static const long ID_STATICTEXT7;
    static const long ID_TEXTCTRL9;
    static const long ID_STATICTEXT8;
    static const long ID_TEXTCTRL10;
    static const long ID_STATICTEXT9;
    static const long ID_TEXTCTRL11;
    static const long ID_STATICTEXT10;
    static const long ID_TEXTCTRL12;
    static const long ID_STATICTEXT11;
    static const long ID_TEXTCTRL13;
    static const long ID_STATICTEXT12;
    static const long ID_TEXTCTRL14;
    static const long ID_BUTTON6;
    static const long ID_BUTTON7;
    static const long ID_BUTTON8;
    static const long ID_BUTTON12;
    static const long ID_PANEL3;
    static const long ID_SLIDER1;
    static const long ID_SLIDER2;
    static const long ID_TEXTCTRL3;
    static const long ID_TEXTCTRL4;
    static const long ID_CHOICE2;
    static const long ID_CHECKBOX4;
    static const long ID_CHECKBOX8;
    static const long ID_BUTTON2;
    static const long ID_BUTTON1;
    static const long ID_BUTTON3;
    static const long ID_CHECKBOX5;
    static const long ID_SLIDER4;
    static const long ID_TEXTCTRL7;
    static const long ID_SLIDER3;
    static const long ID_TEXTCTRL5;
    static const long ID_SLIDER5;
    static const long ID_TEXTCTRL8;
    static const long ID_PANEL2;
    static const long ID_GRID1;
    static const long ID_CHECKBOX7;
    static const long ID_BUTTON4;
    static const long ID_BUTTON5;
    static const long ID_STATICTEXT13;
    static const long ID_TEXTCTRL15;
    static const long ID_PANEL4;
    static const long ID_STATICBITMAP1;
    static const long ID_STATICTEXT3;
    static const long ID_PANEL5;
    static const long ID_NOTEBOOK2;
    static const long ID_TEXTCTRL1;
    static const long ID_STATUSBAR1;
    static const long ID_TIMER1;
    static const long ID_TOOLBAR_DEFAULT;
    static const long ID_TOOLBAR_SALVAR;
    static const long ID_TOOLBAR_CARREGAR;
    static const long ID_TOOLBAR1;
    //*)

    //(*Declarations(serialcomFrame)
    wxSlider* Slider1;
    wxStaticText* StaticText10;
    wxCheckBox* CheckBoxAutoRefreshSensors;
    wxSlider* SliderRoll;
    wxStaticText* StaticText9;
    wxGrid* GridJoy;
    wxCheckBox* CheckBoxEnJoystick;
    wxTextCtrl* TextCtrl4;
    wxPanel* Panel5;
    wxToolBar* ToolBar1;
    wxTextCtrl* Log;
    wxButton* Button4;
    wxSlider* Slider2;
    wxNotebook* Notebook1;
    wxToolBarToolBase* ToolBarItem3;
    wxTextCtrl* TextCtrlTilt;
    wxStaticText* StaticText2;
    wxPanel* Panel4;
    wxCheckBox* CheckBoxRXdata;
    wxSlider* SliderTilt;
    wxButton* Button1;
    wxCheckBox* CheckBoxAutoRefresh;
    wxStaticText* StaticText6;
    wxStaticBitmap* StaticBitmap1;
    wxChoice* ChoicePrg;
    wxTextCtrl* TextCtrlPan;
    wxButton* Button14;
    wxStaticText* StaticText8;
    wxStaticText* StaticText11;
    wxTextCtrl* TextCtrlKp;
    wxCheckBox* CheckBoxJoyServos;
    wxPanel* Panel1;
    wxStaticText* StaticText1;
    wxStaticText* StaticText3;
    wxGrid* Grid1;
    wxTextCtrl* TextCtrlDelayInch;
    wxButton* Button2;
    wxPanel* Panel6;
    wxPanel* Panel3;
    wxTextCtrl* TextCtrlKd;
    wxButton* Button6;
    wxCheckBox* CheckBoxDrvByJoy;
    wxButton* Button10;
    wxToolBarToolBase* ToolBarItem1;
    wxTextCtrl* TextCtrlDelayRead;
    wxButton* Button11;
    wxButton* Button5;
    wxCheckBox* CheckBoxHandBrake;
    wxTextCtrl* TextCtrlRoll;
    wxButton* Button3;
    wxStaticText* StaticText5;
    wxButton* Button7;
    wxStaticText* StaticText7;
    wxStatusBar* StatusBar1;
    wxButton* Button9;
    wxTextCtrl* TextCtrl2;
    wxTextCtrl* TextCtrl1;
    wxStaticText* StaticText12;
    wxTextCtrl* TextCtrlDelayTurn;
    wxPanel* Panel2;
    wxSlider* SliderPan;
    wxTextCtrl* TextCtrlKi;
    wxStaticText* StaticText4;
    wxTextCtrl* TextCtrl3;
    wxChoice* Choice1;
    wxToolBarToolBase* ToolBarItem2;
    wxChoice* Choice2;
    wxButton* Button8;
    wxStaticText* StaticTextJoyButtons;
    wxTimer Timer1;
    wxButton* Button12;
    wxTextCtrl* SendCommandText;
    wxCheckBox* CheckBoxPoll;
    //*)

    DECLARE_EVENT_TABLE()
};

#endif // SERIALCOMMAIN_H
