/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_ACTIVEXBROWSERHOST
#define H_ACTIVEXBROWSERHOST

#include "win_common.h"
#include "COM_config.h"
#include <atlctl.h>
#include "BrowserHostWrapper.h"
#include "APITypes.h"

class ActiveXBrowserHost :
    public FB::BrowserHostWrapper
{
public:
    ActiveXBrowserHost(HWND wnd);
    virtual ~ActiveXBrowserHost(void);
    virtual void ScheduleAsyncCall(void (*func)(void *), void *userData);

    virtual void *getContextID();

    void setWindow(HWND wnd);

protected:
    HWND m_hWnd;

public:
    FB::variant getVariant(const VARIANT *cVar);
    void getComVariant(VARIANT *dest, const FB::variant &var);
};

#endif // H_ACTIVEXBROWSERHOST