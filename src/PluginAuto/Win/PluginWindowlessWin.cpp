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

PluginWindowlessWin::PluginWindowlessWin(const FB::WindowContextWindowless& ctx)
    : m_hdc(ctx.drawable), m_browserHwnd(NULL) {}

PluginWindowlessWin::~PluginWindowlessWin() {}

void PluginWindowlessWin::translateWindowToPlugin(int32_t &x, int32_t &y) const {
    int32_t tempX, tempY;
    tempX = x - m_x;
    tempY = y - m_y;
    x = tempX;
    y = tempY;
}

bool PluginWindowlessWin::HandleEvent(uint32_t event, uint32_t wParam, uint32_t lParam, LRESULT& lRes)
{ 
    {
        LRESULT lres(0);
        WindowsEvent nEvt((HWND)NULL, event, wParam, lParam, lres);
        if (SendEvent(&nEvt)) {
            lRes = nEvt.lRes;
            return TRUE;
        }
    }
    switch(event) {
        case WM_LBUTTONDOWN: 
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONDOWN:
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONDOWN:
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_LBUTTONUP: 
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
            return SendEvent(&ev);
        }
        case WM_RBUTTONUP:
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
            return SendEvent(&ev);
        }
        case WM_MBUTTONUP:
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
            return SendEvent(&ev);
        }
        case WM_MOUSEMOVE:
        {
            int32_t x = GET_X_LPARAM(lParam);
            int32_t y = GET_Y_LPARAM(lParam);
            translateWindowToPlugin(x, y);
            MouseMoveEvent ev(x, y);
            return SendEvent(&ev);
        }
        case WM_PAINT:
        {
            HDC dc = (HDC)wParam;
            if(dc != m_hdc) {
                setHDC(dc);
            }
            RefreshEvent ev;
            if (!SendEvent(&ev)) {
                FB::Rect pos = getWindowPosition();
                ::SetTextAlign(dc, TA_CENTER|TA_BASELINE);
                LPCTSTR pszText = _T("FireBreath Plugin");
                ::TextOut(dc, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));
            }
            return 0;
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
    return 0;
}

FB::Rect PluginWindowlessWin::getWindowPosition() const {
    int32_t top = m_y;
    int32_t left = m_x;
    int32_t bottom = m_y + m_height;
    int32_t right = m_x + m_width;
    FB::Rect r = { top, left, bottom, right };
    return r;
}

void PluginWindowlessWin::setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    m_x = x;
    m_y = y;
    m_height = height;
    m_width = width;

	ResizedEvent ev;
	SendEvent(&ev);  //notify the plugin the window has changed position/size
}

void PluginWindowlessWin::setWindowPosition(FB::Rect pos) {
    m_x = pos.left;
    m_y = pos.top;
    m_height = pos.bottom - m_y;
    m_width = pos.right - m_x;

	ResizedEvent ev;
	SendEvent(&ev);  //notify the plugin the window has changed position/size
}

FB::Rect FB::PluginWindowlessWin::getWindowClipping() const {
    FB::Rect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

void PluginWindowlessWin::setWindowClipping(int32_t top, int32_t left, int32_t bottom, int32_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

void PluginWindowlessWin::setWindowClipping(FB::Rect clip) {
    m_clipTop = clip.top;
    m_clipLeft = clip.left;
    m_clipBottom = clip.bottom;
    m_clipRight = clip.right;
}

void FB::PluginWindowlessWin::InvalidateWindow() const
{
    if (m_invalidateWindow)
        m_invalidateWindow(0, 0, getWindowWidth(), getWindowHeight());
}

