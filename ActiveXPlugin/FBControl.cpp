/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FBControl.cpp : Implementation of CFBControl
#include "win_common.h"
#include "FBControl.h"

#include "WindowsEventHandler.h"

// CFBControl
BOOL CFBControl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    if (uMsg == WM_CREATE) {
        int i = 2;
        i++;
    }
    BOOL bHandled = TRUE;
    (hWnd);
    (uMsg);
    (wParam);
    (lParam);
    (lResult);
    (bHandled);
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

        bHandled = WindowsEventHandler::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);

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
