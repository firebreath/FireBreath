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
#include "win_common.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "AsyncFunctionCall.h"
#include "logging.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include <ShlGuid.h>
#include <string>

#include "WinMessageWindow.h"

extern HINSTANCE gInstance;

static boost::recursive_mutex _windowMapMutex;
static std::map<HWND, FB::WinMessageWindow*> _windowMap;

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
    _windowMap[messageWin] = this;
    m_hWnd = messageWin;
    boost::recursive_mutex::scoped_lock _l(_windowMapMutex);
    winProc = boost::bind(&FB::WinMessageWindow::DefaultWinProc, this, _1, _2, _3, _4, _5);
}

LRESULT CALLBACK FB::WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT lres = S_OK;
    FB::WinMessageWindow* self(NULL);
    {
        boost::recursive_mutex::scoped_lock _l(_windowMapMutex);
        if (_windowMap.find(hWnd) != _windowMap.end())
            self = _windowMap[hWnd];
    }
    if (!self || !self->winProc(hWnd, uMsg, wParam, lParam, lres)) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    } else {
        return lres;
    }
}

HWND FB::WinMessageWindow::getHWND()
{
    return m_hWnd;
}

FB::WinMessageWindow::~WinMessageWindow()
{
    boost::recursive_mutex::scoped_lock _l(_windowMapMutex);
    _windowMap.erase(m_hWnd);
    ::DestroyWindow(m_hWnd);
}

bool FB::WinMessageWindow::DefaultWinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult )
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        FBLOG_TRACE("PluginWindow", "Running async function call");
        FB::AsyncFunctionCall *evt = static_cast<FB::AsyncFunctionCall*>((void*)lParam);
        evt->func(evt->userData);
        delete evt;
        lResult = S_OK;
        return true;
    }
    return false;
}
