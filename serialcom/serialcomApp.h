/***************************************************************
 * Name:      serialcomApp.h
 * Purpose:   Defines Application Class
 * Author:    Mauricio Bieze Stefani ()
 * Created:   2010-01-07
 * Copyright: Mauricio Bieze Stefani ()
 * License:
 **************************************************************/

#ifndef SERIALCOMAPP_H
#define SERIALCOMAPP_H

#include <wx/app.h>

class serialcomApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // SERIALCOMAPP_H
