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
#include <wx/menu.h>
#include <wx/textctrl.h>
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
        //*)

        //(*Identifiers(serialcomFrame)
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
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
        wxTextCtrl* Log;
        wxNotebook* Notebook1;
        wxPanel* Panel1;
        wxStatusBar* StatusBar1;
        wxPanel* Panel2;
        wxTimer Timer1;
        wxTextCtrl* SendCommandText;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SERIALCOMMAIN_H
