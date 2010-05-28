/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "Win/win_common.h"
#include "Win/KeyCodesWin.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginWindowWin.h"

    
using namespace FB;

PluginWindowWin::PluginWindowMap FB::PluginWindowWin::m_windowMap;

PluginWindowWin::PluginWindowWin(HWND hWnd) : m_hWnd(hWnd), lpOldWinProc(NULL)
{
    // subclass window so we can intercept window messages 
    lpOldWinProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWindowWin::_WinProc);

    // associate window with this object so that we can route events properly
    m_windowMap[static_cast<void*>(m_hWnd)] = this;
}

PluginWindowWin::~PluginWindowWin()
{
    // Unsubclass the window so that everything is as it was before we got it
    SubclassWindow(m_hWnd, lpOldWinProc);

    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(m_hWnd));
    if (it != m_windowMap.end()) 
        m_windowMap.erase(it);
}

bool PluginWindowWin::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lRes)
{
    lRes = 0;
    // Before all else, give the plugin a chance to handle the platform specific event
    WindowsEvent ev(hWnd, uMsg, wParam, lParam, lRes);
    if (SendEvent(&ev)) {
        return true;
    }

    switch(uMsg) {
        case WM_LBUTTONDOWN: 
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, 
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_RBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right,
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_MBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle,
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_LBUTTONUP: 
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_RBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_MBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_MOUSEMOVE:
        {
            SetFocus( m_hWnd ); //get key focus, as the mouse is over our region
            MouseMoveEvent ev(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return SendEvent(&ev);
        }
        case WM_PAINT:
        {
            RefreshEvent ev;
            return SendEvent(&ev);
        }
        case WM_TIMER:
        {
            TimerEvent ev((unsigned int)wParam, (void*)lParam);
            return SendEvent(&ev);
        }
        case WM_KEYUP:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyUpEvent ev(fb_key, (unsigned int)wParam);
            return SendEvent(&ev);
        }
        case WM_KEYDOWN:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyDownEvent ev(fb_key, (unsigned int)wParam);
            return SendEvent(&ev);
        }   
    }

    if (CustomWinProc(hWnd, uMsg, wParam, lParam, lRes))
        return true;

    return false;
}

LRESULT CALLBACK PluginWindowWin::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        WINDOWS_ASYNC_EVENT *evt = static_cast<WINDOWS_ASYNC_EVENT*>((void*)lParam);
        evt->func(evt->userData);
        delete evt;
        return S_OK;
    } 

    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(hWnd));
    if (it == m_windowMap.end()) 
        return 0;
    PluginWindowWin *win = it->second;


    LRESULT lResult(0);
    // Try to handle the event through the plugin instace; if that doesn't work, handle it through the default winproc
    if (win->WinProc(hWnd, uMsg, wParam, lParam, lResult))
        return lResult;
    else
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void PluginWindowWin::InvalidateWindow()
{
    InvalidateRect(m_hWnd, NULL, true);
}
