/***************************************************************
 * Name:      serialcomMain.h
 * Purpose:   Defines Application Frame
 * Author:    Mauricio Bieze Stefani ()
 * Created:   2010-01-07
 * Copyright: Mauricio Bieze Stefani ()
 * License:
 **************************************************************/

#ifndef SERIALCOMMAIN_H
#define SERIALCOMMAIN_H

//(*Headers(serialcomFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include <MBSUtil.h>

class serialcomFrame: public wxFrame
{
    public:

        serialcomFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~serialcomFrame();

		SerialPort serialPort;
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
        //*)

        //(*Identifiers(serialcomFrame)
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT2;
        static const long ID_SLIDER1;
        static const long ID_SLIDER2;
        static const long ID_TEXTCTRL3;
        static const long ID_TEXTCTRL4;
        static const long ID_PANEL2;
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
        wxSlider* Slider2;
        wxNotebook* Notebook1;
        wxStaticText* StaticText2;
        wxMenu* Menu3;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxMenuItem* MenuItem3;
        wxStatusBar* StatusBar1;
        wxTextCtrl* TextCtrl2;
        wxTextCtrl* TextCtrl1;
        wxPanel* Panel2;
        wxTimer Timer1;
        wxTextCtrl* SendCommandText;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SERIALCOMMAIN_H
