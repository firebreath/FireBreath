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
#include <functional>
#include <mutex>
#include <map>
#include "AsyncFunctionCall.h"
#include "logging.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include <ShlGuid.h>
#include <string>

#include "WinMessageWindow.h"

// thanks http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

class WindowsClassRegistration
{
    ATOM m_atom;
    DWORD m_err;
    const wchar_t* m_className;

public:
    WindowsClassRegistration(const wchar_t* classname)
        : m_err(0)
        , m_className(classname)
    {
        WNDCLASSEX wc;
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = &FB::WinMessageWindow::_WinProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = HINST_THISCOMPONENT;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = m_className;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hIconSm = NULL;
        wc.hbrBackground = NULL;
        m_atom = ::RegisterClassEx(&wc);

        if (!m_atom) {
            m_err = ::GetLastError();
        }
    }

    ~WindowsClassRegistration()
    {
        if (0 == m_err) {
            UnregisterClass(MAKEINTATOM(m_atom), HINST_THISCOMPONENT);
        }
    }

    ATOM atom() const
    {
        if (m_err) {
            std::stringstream ss;
            ss << "Could not register window class, error:" << m_err;
            throw std::runtime_error(ss.str());
        }
        return m_atom;
    }
};

static std::recursive_mutex _windowMapMutex;
static std::map<HWND, FB::WinMessageWindow*> _windowMap;
static WindowsClassRegistration _winclass(L"FBEventWindow");

FB::WinMessageWindow::WinMessageWindow() {
    DWORD err(0);

    static int count(0);
    std::wstring winName = L"FireBreathEventWindow" + std::to_wstring(count);
    ++count;

    HWND messageWin = CreateWindowEx(
        WS_OVERLAPPED,
        MAKEINTATOM(_winclass.atom()),
        winName.c_str(),
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, HINST_THISCOMPONENT, NULL);
    if (!messageWin) {
        err = ::GetLastError();
        throw std::runtime_error("Could not create Message Window");
    }
    _windowMap[messageWin] = this;
    m_hWnd = messageWin;
    std::unique_lock<std::recursive_mutex> _l(_windowMapMutex);

    using namespace std::placeholders;
    winProc = std::bind(&FB::WinMessageWindow::DefaultWinProc, this, _1, _2, _3, _4, _5);
}

LRESULT CALLBACK FB::WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT lres = S_OK;
    FB::WinMessageWindow* self(NULL);
    {
        std::unique_lock<std::recursive_mutex> _l(_windowMapMutex);
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
    std::unique_lock<std::recursive_mutex> _l(_windowMapMutex);
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
