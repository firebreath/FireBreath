#include "WinDrawingTest.h"

WinDrawingTest::WinDrawingTest() :
    DrawingTest(),
    m_window(0),
    m_red(0)
{
    //Create and setup the timer object to call the update function every 40 ms
    m_timer = FB::Timer::getTimer(40, true, boost::bind(&X11DrawingTest::update, this));
}

/**
 * Function that changes the colour to draw and invalidates the screen
 */
void WinDrawingTest::update()
{
    // Increment the red component by 10 and reset it back to 0 if it exceeds 255
    m_red += 10;
    if(m_red > 255) {
        m_red = 0;
    }

    //if we have a window available Invalidate it so another Expose event gets raised
    if(this->m_window) {
        this->m_window->InvalidateWindow();
    }
}

/**
 * Called when a window gets attached to the plugin which stores the pointer to the
 * window in m_window and then starts the timer
 */
bool WinDrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
{
    bool result = DrawingTest::onWindowAttached(evt, window);

    // The window is attached, store the pointer to the window so
    // the subclasses can invalidate it in the update thread
    m_window = window;

    //start the timer that updates the display
    m_timer->start();

    return result;
}


/**
 * Called when a window is going to be detached from the plugin. This sets m_window to null and
 * then stops the timer
 */
bool WinDrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    // The window is about to be detached. Clear the window pointer
    // so the update thread doesn't attempt to invalidate it
    m_window = NULL;

    //stop the timer that updates the display
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

/**
 * Called when the window needs to be redrawn. It handles the drawing to the window using GDI.
 * All it does is colour the whole window a single colour
 */
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
