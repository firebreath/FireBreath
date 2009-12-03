/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 5, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef  H_WINDOWSEVENTHANDLER
#define  H_WINDOWSEVENTHANDLER

#include "Win/win_common.h"

#define WM_ASYNCTHREADINVOKE    WM_USER + 1

struct WINDOWS_ASYNC_EVENT
{
    WINDOWS_ASYNC_EVENT(void (*f)(void *), void *ud) 
        : func(f), userData(ud) { }
    void (*func)(void *);
    void *userData;
};

class WindowsEventHandler
{
public:
    static bool ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
};

#endif
