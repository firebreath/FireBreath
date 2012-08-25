#include "MacDrawingTest.h"

MacDrawingTest::MacDrawingTest() :
        DrawingTest(),
        m_red(0), m_green(0), m_blue(0)
{
    m_timer = FB::Timer::getTimer(40, true, boost::bind(&X11DrawingTest::update, this));
}

void MacDrawingTest::update()
{
    m_red += 1;
    if(m_red > 255) {
        m_red = 0;
    }

    if(this->m_window) {
        this->m_window->InvalidateWindow();
    }
}

bool MacDrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
{
    bool result = DrawingTest::onWindowAttached(evt, window);

    m_timer->start();

    return result;
}

bool MacDrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

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
