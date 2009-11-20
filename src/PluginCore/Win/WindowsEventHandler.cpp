/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 5, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "WindowsEventHandler.h"

bool WindowsEventHandler::ProcessWindowMessage(HWND hWnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{

    switch(uMsg) {

        case WM_ASYNCTHREADINVOKE: {
            WINDOWS_ASYNC_EVENT *evt = static_cast<WINDOWS_ASYNC_EVENT*>((void*)lParam);
            evt->func(evt->userData);
            } return true;

        default:
            return false;
    }
}