/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginWindowWin.h"
    
#include "windows.h"
#include "windowsx.h"

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

bool PluginWindowWin::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lRes)
{
    switch(uMsg) {
        case WM_LBUTTONDOWN: 
            return SendEvent(&MouseDownEvent(MouseButtonEvent::MouseButton_Left,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
        case WM_RBUTTONDOWN:
            return SendEvent(&MouseDownEvent(MouseButtonEvent::MouseButton_Right,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
        case WM_MBUTTONDOWN:
            return SendEvent(&MouseDownEvent(MouseButtonEvent::MouseButton_Middle,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
        case WM_LBUTTONUP: 
            return SendEvent(&MouseUpEvent(MouseButtonEvent::MouseButton_Left,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
        case WM_RBUTTONUP:
            return SendEvent(&MouseUpEvent(MouseButtonEvent::MouseButton_Right,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
        case WM_MBUTTONUP:
            return SendEvent(&MouseUpEvent(MouseButtonEvent::MouseButton_Middle,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;

        case WM_MOUSEMOVE:
            return SendEvent(&MouseMoveEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;
    }

    if (CustomWinProc(hWnd, uMsg, wParam, lParam, lRes))
        return true;

    // Default event if nothing else is done with it
    WindowsEvent evt(hWnd, uMsg, wParam, lParam);
    return this->SendEvent(&evt);
}

LRESULT CALLBACK PluginWindowWin::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        WINDOWS_ASYNC_EVENT *evt = static_cast<WINDOWS_ASYNC_EVENT*>((void*)lParam);
        evt->func(evt->userData);
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
