/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FBControl.cpp : Implementation of CFBControl
#include "FBControl.h"

// CFBControl
BOOL CFBControl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    if (uMsg == WM_CREATE) {
        int i = 2;
        i++;
    }
    BOOL bHandled = FALSE;
    switch(dwMsgMapID)
    {
    case 0: {
        // WM_CREATE is the only message we handle here
        switch(uMsg)
        {
        case WM_CREATE:
            lResult = OnCreate(uMsg, wParam, lParam, bHandled);
            if(bHandled)
                return TRUE;
            break;

        case WM_DESTROY:
        case WM_CLOSE:
            shutdown();
            break;
        }

        if (bHandled)
            return TRUE;
        else if(CComControl<CFBControl>::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
            return TRUE;
        else if(DefaultReflectionHandler(hWnd, uMsg, wParam, lParam, lResult))
            return TRUE;
        } break;
    
    }
    return FALSE;
}
