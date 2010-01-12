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
#include <wx/statusbr.h>
//*)

#include <MBSUtil.h>

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
        //*)

        //(*Identifiers(serialcomFrame)
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
        static const long ID_NOTEBOOK1;
        static const long ID_TEXTCTRL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(serialcomFrame)
        wxTextCtrl* Log;
        wxNotebook* Notebook1;
        wxPanel* Panel1;
        wxStatusBar* StatusBar1;
        wxTextCtrl* SendCommandText;
        //*)

        DECLARE_EVENT_TABLE()

        SerialPort serialPort;
};

#endif // SERIALCOMMAIN_H