#include "X11DrawingTest.h"

X11DrawingTest::X11DrawingTest() :
        DrawingTest(),
        m_red(0)
{
    m_timer = FB::Timer::getTimer(40, true, boost::bind(&X11DrawingTest::update, this));
}

void X11DrawingTest::update()
{
    m_red += 10;
    if(m_red > 255) {
        m_red = 0;
    }

    if(this->m_window) {
        this->m_window->InvalidateWindow();
    }
}

bool X11DrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
{
    bool result = DrawingTest::onWindowAttached(evt, window);

    m_timer->start();

    return result;
}

bool X11DrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

bool X11DrawingTest::onX11Event(FB::X11Event *evt, FB::PluginWindow *pluginWindow)
{
    FB::PluginWindowX11* window = reinterpret_cast<FB::PluginWindowX11*>(pluginWindow);

    if(!window) {
        return false;
    }

    GtkWidget* canvas = window->getWidget();

    if (evt->m_event->type == GDK_EXPOSE) {
        return this->onGdkExpose(canvas);
    }

    return true;
}

bool X11DrawingTest::onGdkExpose(GtkWidget *canvas)
{
    cairo_t *cr;
    cr = gdk_cairo_create (gtk_widget_get_window(canvas));

    float red = ((float)m_red / 255);

    cairo_rectangle(cr, 0, 0, canvas->allocation.width, canvas->allocation.height);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, red, 0.5, 0.5);
    cairo_fill(cr);

    cairo_destroy (cr);

    return true;
}
