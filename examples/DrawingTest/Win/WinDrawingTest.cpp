#include "WinDrawingTest.h"

WinDrawingTest::WinDrawingTest() :
        DrawingTest(),
        m_red(0), m_green(0), m_blue(0)
{
    m_timer = FB::Timer::getTimer(40, true, boost::bind(&X11DrawingTest::update, this));
}

void WinDrawingTest::update()
{
    m_red += 1;
    if(m_red > 255) {
        m_red = 0;
    }

    if(this->m_window) {
        this->m_window->InvalidateWindow();
    }
}

bool WinDrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
{
    bool result = DrawingTest::onWindowAttached(evt, window);

    m_timer->start();

    return result;
}

bool WinDrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

bool WinDrawingTest::onWindowRefresh(FB::RefreshEvent *evt, FB::PluginWindow *pluginWindow)
{
    FB::PluginWindowWin* window = reinterpret_cast<FB::PluginWindowWin*>(pluginWindow);

    if(!window) {
        return true;
    }

    HWND hwnd = window->getHWND();

    HDC         hDC;
    PAINTSTRUCT Ps;
    HPEN		hPen;
    HBRUSH		hBrush;

    hDC = BeginPaint(hwnd, &Ps);

    hPen = CreatePen(PS_SOLID, 1, RGB(m_red, 128, 128));
    hBrush = CreateSolidBrush(RGB(m_red, 128, 128));

    SelectObject(hDC, hPen);
    SelectObject(hDC, hBrush);

    Rectangle(hDC, 0, 0, window->getWindowWidth(), window->getWindowHeight());

    EndPaint(hwnd, &Ps);
    return true;
}
