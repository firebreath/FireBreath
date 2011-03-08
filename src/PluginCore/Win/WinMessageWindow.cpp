/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Jan 5, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "win_targetver.h"
#include <windows.h>
#include <ShlGuid.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include "AsyncFunctionCall.h"
#include "logging.h"

#include "WinMessageWindow.h"

extern HINSTANCE gInstance;

FB::WinMessageWindow::WinMessageWindow() {
    WNDCLASSEX wc;
    DWORD err(0);
    ATOM clsAtom(NULL);
    static int count(0);

    std::wstring winName = L"FireBreathEventWindow" + boost::lexical_cast<std::wstring>(count);
    std::wstring className = L"FBEventWindow" + boost::lexical_cast<std::wstring>(count);
    ++count;

    if (!clsAtom) {
        //Step 1: Registering the Window Class
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = &WinMessageWindow::_WinProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = gInstance;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = className.c_str();
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hIconSm = NULL;
        wc.hbrBackground = NULL;
    
        if (!(clsAtom = ::RegisterClassEx(&wc))) {
            err = ::GetLastError();
            if (err != ERROR_CLASS_ALREADY_EXISTS) {
                throw std::runtime_error("Could not register window class");
            }
        }
    }
    // Step 2: Creating the Window
    HWND messageWin = CreateWindowEx(
        WS_OVERLAPPED,
        className.c_str(),
        winName.c_str(),
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, gInstance, NULL);
    if (!messageWin) {
        err = ::GetLastError();
        throw std::runtime_error("Could not create Message Window");
    }
    m_hWnd = messageWin;
}

LRESULT CALLBACK FB::WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        FBLOG_TRACE("PluginWindow", "Running async function call");
        FB::AsyncFunctionCall *evt = static_cast<FB::AsyncFunctionCall*>((void*)lParam);
        evt->func(evt->userData);
        delete evt;
        return S_OK;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND FB::WinMessageWindow::getHWND()
{
    return m_hWnd;
}

FB::WinMessageWindow::~WinMessageWindow()
{
    ::DestroyWindow(m_hWnd);
}

