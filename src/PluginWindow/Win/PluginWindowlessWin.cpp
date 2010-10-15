#include "PluginWindowlessWin.h"

using namespace std; using namespace FB;

PluginWindowlessWin::PluginWindowlessWin(HDC hdc) : m_hdc(hdc) {}

PluginWindowlessWin::~PluginWindowlessWin() {}

int16_t PluginWindowlessWin::HandleEvent(NPEvent* evt) 
{
    switch(evt->event) {
        case WM_LBUTTONDOWN: 
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, 
                              GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_RBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right,
                              GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_MBUTTONDOWN:
        {
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle,
                              GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_LBUTTONUP: 
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left,
                            GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_RBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right,
                            GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_MBUTTONUP:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle,
                            GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_MOUSEMOVE:
        {
            MouseMoveEvent ev(GET_X_LPARAM(evt->lParam), GET_Y_LPARAM(evt->lParam));
            return SendEvent(&ev);
        }
        case WM_PAINT:
        {
            NPRect* r = (NPRect*)evt->lParam;
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
    NPRect r = { m_y, m_x, m_y + m_height, m_x + m_width };
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
