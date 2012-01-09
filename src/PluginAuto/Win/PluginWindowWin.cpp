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

#include "win_common.h"
#include <ShlGuid.h>
#include "logging.h"
#include "Win/KeyCodesWin.h"
#include "AsyncFunctionCall.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginWindowWin.h"

#include "ConstructDefaultPluginWindows.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#define WM_ASYNCTHREADINVOKE    WM_USER + 1

using namespace FB;

extern HINSTANCE gInstance;

PluginWindowWin::PluginWindowMap FB::PluginWindowWin::m_windowMap;

FB::PluginWindowWin* FB::createPluginWindowWin(const FB::WindowContextWin& ctx)
{
    return new PluginWindowWin(ctx);
}

PluginWindowWin::PluginWindowWin(const WindowContextWin& ctx)
  : m_hWnd(ctx.handle)
  , m_browserhWnd(NULL)
  , lpOldWinProc(NULL)
  , m_callOldWinProc(false)
{
    // subclass window so we can intercept window messages 
    lpOldWinProc = SubclassWindow(m_hWnd, (WNDPROC)&PluginWindowWin::_WinProc);
    // associate window with this object so that we can route events properly
    m_windowMap[static_cast<void*>(m_hWnd)] = this;
}

PluginWindowWin::~PluginWindowWin()
{
    // Unsubclass the window so that everything is as it was before we got it
    // Some browsers, like Chrome, unsubclass it for us and then proxy to us. Creepy, no?
    WNDPROC current_wnd_proc = reinterpret_cast<WNDPROC>(
        GetWindowLongPtr(m_hWnd, GWLP_WNDPROC));
    if (current_wnd_proc == &PluginWindowWin::_WinProc)
        SubclassWindow(m_hWnd, lpOldWinProc);

    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(m_hWnd));
    if (it != m_windowMap.end()) 
        m_windowMap.erase(it);
}

bool PluginWindowWin::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lRes)
{
    lRes = 0;
    // Before all else, give the plugin a chance to handle the platform specific event;
    // Then give the custom winproc the chance
    if (CustomWinProc(hWnd, uMsg, wParam, lParam, lRes))
        return true;
    WindowsEvent ev(hWnd, uMsg, wParam, lParam, lRes);
    if (SendEvent(&ev)) {
        return true;
    }

	unsigned int modifierState = (GetKeyState(VK_SHIFT) & 0x8000) != 0 ? MouseButtonEvent::ModifierState_Shift : 0;
	modifierState += (GetKeyState(VK_CONTROL) & 0x8000) != 0 ? MouseButtonEvent::ModifierState_Control : 0;
	modifierState += (GetKeyState(VK_MENU) & 0x8000) != 0 ? MouseButtonEvent::ModifierState_Menu : 0;
    switch(uMsg) {
		case WM_MOUSEACTIVATE:
		{
			///
			/// @note: if you handle WM_MOUSEACTIVATE in WindowsEvent, you 
			/// MUST return correct lRes's value and correct return value 
			/// for SendEvent(). If do not, probably the plugin does not 
			/// properly receive the input focus and keystrokes. Handle this 
			/// message carefully!
			///
#ifdef _DEBUG
			std::stringstream ss;
			FBLOG_INFO("PluginWindowWin::WinProc: ",
                "WM_MOUSEACTIVATE Forcing setting focus to hWnd=0x"
                << std::hex << hWnd << " and returning MA_ACTIVATE value.");
#endif
			SetFocus( m_hWnd ); // get key focus when the mouse is clicked on our plugin
			lRes = MA_ACTIVATE; // returns activation code (for normal scenarios should return
                                //   MA_ACTIVATE value, please review msdn documentation about
                                //   available return values)
			return true;
		}
		case WM_GETDLGCODE:
		{
			lRes =	DLGC_WANTALLKEYS	// All keyboard input.
				|	DLGC_WANTARROWS		// Direction keys.
				|	DLGC_WANTCHARS		// WM_CHAR messages.
				|	DLGC_WANTMESSAGE	// All keyboard input (the application passes this message
                                        //   in the MSG structure to the control).
				|	DLGC_WANTTAB;		// TAB key.
#ifdef _DEBUG
			std::stringstream ss;
			FBLOG_INFO("PluginWindowWin::WinProc: ",
                "WM_GETDLGCODE Forcing to return " << lRes
                << " value for hWnd=0x" << std::hex << hWnd);
#endif
			return true;
		}
        case WM_LBUTTONDOWN: 
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, 
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right,
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_MBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle,
                              GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_LBUTTONUP: 
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_RBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_MBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle,
                            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), modifierState);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_MOUSEMOVE:
        {
            MouseMoveEvent ev(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_MOUSEWHEEL:
        {
            MouseScrollEvent ev(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                                0, GET_WHEEL_DELTA_WPARAM(wParam));
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_PAINT:
        {
            RefreshEvent ev;
            if (!SendEvent(&ev)) {
                HDC hdc;
                PAINTSTRUCT ps;
                hdc = BeginPaint(m_hWnd, &ps);
                // Release the device context
                EndPaint(m_hWnd, &ps);
                return true;
            }
            break;
        }
        case WM_TIMER:
        {
            TimerEvent ev((unsigned int)wParam, (void*)lParam);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_SETCURSOR:
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            break;
        case WM_KEYUP:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyUpEvent ev(fb_key, (unsigned int)wParam);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_KEYDOWN:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)wParam);
            KeyDownEvent ev(fb_key, (unsigned int)wParam);
            if(SendEvent(&ev))
                return true;
            break;
        }
        case WM_SIZE:
        {
            ResizedEvent ev;
            if(SendEvent(&ev))
                return true;
            break;
        }
    }

    return false;
}

LRESULT CALLBACK PluginWindowWin::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(hWnd));
    if (it == m_windowMap.end()) 
        // This could happen if we're using this as a message-only window
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    PluginWindowWin *win = it->second;


    LRESULT lResult(0);
    // Try to handle the event through the plugin instace; if that doesn't work, handle it through the default winproc
    if (win->WinProc(hWnd, uMsg, wParam, lParam, lResult))
        return lResult;
    else if (win->m_callOldWinProc)
        return win->lpOldWinProc(hWnd, uMsg, wParam, lParam);
    else
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    return 0;
}

void PluginWindowWin::InvalidateWindow() const
{
    InvalidateRect(m_hWnd, NULL, true);
}

FB::Rect FB::PluginWindowWin::getWindowPosition() const
{
    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    FB::Rect out = {
        boost::numeric_cast<int32_t>(rect.top),
        boost::numeric_cast<int32_t>(rect.left),
        boost::numeric_cast<int32_t>(rect.bottom),
        boost::numeric_cast<int32_t>(rect.right)};
    return out;
}

FB::Rect FB::PluginWindowWin::getWindowClipping() const
{
    return getWindowPosition();
}

uint32_t FB::PluginWindowWin::getWindowWidth() const
{
    FB::Rect pos = getWindowPosition();
    return pos.right - pos.left;
}

uint32_t FB::PluginWindowWin::getWindowHeight() const
{
    FB::Rect pos = getWindowPosition();
    return pos.bottom - pos.top;
}

