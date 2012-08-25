#include "X11DrawingTest.h"

X11DrawingTest::X11DrawingTest() :
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
void X11DrawingTest::update()
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
bool X11DrawingTest::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window)
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
bool X11DrawingTest::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window)
{
    // The window is about to be detached. Clear the window pointer
    // so the update thread doesn't attempt to invalidate it
    m_window = NULL;

    //stop the timer that updates the display
    m_timer->stop();

    return DrawingTest::onWindowDetached(evt, window);
}

/**
 * Called when an X11 Event is raised. All we care about is the Expose event which tells us to
 * render. We pass that off to onGdkExpose
 */
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

/**
 * Handles the drawing to the window using Cairo. All it does is colour the
 * whole window a single colour
 */
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
