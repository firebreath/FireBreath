#include "MacDrawingTest.h"

MacDrawingTest::MacDrawingTest() :
    DrawingTest(),
    m_window(0),
    m_red(0)
{
    //Create and setup the timer object to call the update function every 40 ms
    m_timer = FB::Timer::getTimer(40, true, boost::bind(&MacDrawingTest::update, this));
}

/**
 * Function that changes the colour to draw and invalidates the screen
 */
void MacDrawingTest::update()
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
bool MacDrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
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
bool MacDrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    // The window is about to be detached. Clear the window pointer
    // so the update thread doesn't attempt to invalidate it
    m_window = NULL;

    //stop the timer that updates the display
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

/**
 * Called when the window needs to be redrawn. It Handles the drawing to the window using CoreGraphics.
 * All it does is colour the whole window a single colour
 */
bool MacDrawingTest::onDrawCG(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG *window)
{
    if(!window) {
        return false;
    }

    FB::Rect bounds(evt->bounds);
    FB::Rect clip(evt->clip);
    CGContextRef cgContext(evt->context);

    short width = bounds.right - bounds.left;
    short height = bounds.bottom - bounds.top;

    CGContextSaveGState(cgContext);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGContextSetFillColorSpace(cgContext, colorspace);

    float red = ((float)m_red / 255);

    CGFloat color1[] = { red, 0.5, 0.5, 1.0 };
    CGContextSetFillColor(cgContext, color1);
    CGRect rect1 = { {0, 0}, {width, height} };
    CGContextFillRect(cgContext, rect1);

    CGColorSpaceRelease(colorspace);
    CGContextRestoreGState(cgContext);

    return true;
}
