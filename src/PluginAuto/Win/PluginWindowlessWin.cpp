#include "PluginWindowlessWin.h"
#include "ConstructDefaultPluginWindows.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

using namespace std; using namespace FB;

FB::PluginWindowlessWin* FB::createPluginWindowless(const FB::WindowContextWindowless& ctx)
{
    return new PluginWindowlessWin(ctx);
}

PluginWindowlessWin::PluginWindowlessWin(const FB::WindowContextWindowless& ctx) : m_hdc(ctx.drawable) {}

PluginWindowlessWin::~PluginWindowlessWin() {}

void PluginWindowlessWin::translateWindowToPlugin(int &x, int &y) {
    int tempX, tempY;
    tempX = x - m_x;
    tempY = y - m_y;
    x = tempX;
    y = tempY;
}

int16_t PluginWindowlessWin::HandleEvent(NPEvent* evt) 
{
    switch(evt->event) {
        case WM_LBUTTONDOWN: 
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONDOWN:
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONDOWN:
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_LBUTTONUP: 
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONUP:
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONUP:
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(evt->lParam);
            int y = GET_Y_LPARAM(evt->lParam);
            translateWindowToPlugin(x, y);
            MouseMoveEvent ev(x, y);
            return SendEvent(&ev);
        }
        case WM_PAINT:
        {
            HDC dc = (HDC)evt->wParam;
            if(dc != m_hdc) {
                setHDC(dc);
            }
            RefreshEvent ev;
            SendEvent(&ev);
            return 0;
        }
        case WM_KEYUP:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)evt->wParam);
            KeyUpEvent ev(fb_key, (unsigned int)evt->wParam);
            return SendEvent(&ev);
        }
        case WM_KEYDOWN:
        {
            FBKeyCode fb_key = WinKeyCodeToFBKeyCode((unsigned int)evt->wParam);
            KeyDownEvent ev(fb_key, (unsigned int)evt->wParam);
            return SendEvent(&ev);
        }   
    }
    return 0;
}

void invalidateFunc(void* userData) {
    PluginWindowlessWin* window = reinterpret_cast<PluginWindowlessWin*>(userData);
    if(window != NULL) {
        window->InvalidateWindow();
    }
}

void PluginWindowlessWin::InvalidateWindow() {
    // Invalidate the whole plugin window (plugin coordinates)
    NPRect r = { 0, 0, m_height, m_width };
    if(m_npHost != NULL) {
        m_npHost->InvalidateRect(&r);
    }
}

void PluginWindowlessWin::AsyncInvalidateWindow() {
    if(m_npHost != NULL) {
        m_npHost->ScheduleAsyncCall(invalidateFunc, this);
    }
}

NPRect PluginWindowlessWin::getWindowPosition() {
    int top = m_y;
    int left = m_x;
    int bottom = m_y + m_height;
    int right = m_x + m_width;
    if(top < 0) { top = 0; }
    if(left < 0) { left = 0; }
    NPRect r = { top, left, bottom, right };
    return r;
}

void PluginWindowlessWin::setWindowPosition(int x, int y, int width, int height) {
    m_x = x;
    m_y = y;
    m_height = height;
    m_width = width;
}

void PluginWindowlessWin::setWindowPosition(NPRect pos) {
    m_x = pos.left;
    m_y = pos.top;
    m_height = pos.bottom - m_y;
    m_width = pos.right - m_x;
}

NPRect FB::PluginWindowlessWin::getWindowClipping() {
    NPRect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

void PluginWindowlessWin::setWindowClipping(int top, int left, int bottom, int right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

void PluginWindowlessWin::setWindowClipping(NPRect clip) {
    m_clipTop = clip.top;
    m_clipLeft = clip.left;
    m_clipBottom = clip.bottom;
    m_clipRight = clip.right;
}

HWND PluginWindowlessWin::getHWND() {
    if(m_npHost != NULL) {
        HWND hwnd;
        m_npHost->GetValue(NPNVnetscapeWindow, (void*)&hwnd);
        return hwnd;
    }
    return NULL;
}
