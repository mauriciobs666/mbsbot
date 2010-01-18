/***************************************************************
 * Name:      serialcomApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Mauricio Bieze Stefani ()
 * Created:   2010-01-07
 * Copyright: Mauricio Bieze Stefani ()
 * License:
 **************************************************************/

#include "serialcomApp.h"

//(*AppHeaders
#include "serialcomMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(serialcomApp);

bool serialcomApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        serialcomFrame* Frame = new serialcomFrame(0);
        Frame->Show();
        SetTopWindow(Frame);

        if(argc)
        {
            wxString tmp(argv[1]);
            Frame->serialPort.init(tmp.mb_str(wxConvUTF8));
        }
    }
    //*)
    return wxsOK;

}
