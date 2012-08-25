#include "../DrawingTest.h"

#include <PluginWindowX11.h>
#include <PluginEvents/DrawingEvents.h>
#include <PluginEvents/X11Event.h>
#include <boost/bind.hpp>
#include <Timer.h>

/**
 * Class X11DrawingTest extends from DrawingTest and implements all rendering
 * in an X11 environment. It does this by listening for X11Event's being raised
 * (specifically GDK Expose events) and then rendering to the Window we've been
 * given.
 *
 * It also contains a timer that calls a function every 40ms (25 fps) that
 * changes the colour to render and then invalidates the window so that another
 * Expose event is raised.
 */
class X11DrawingTest : public DrawingTest
{
public:
    X11DrawingTest();

    /* Create a plugin event map to get X11 events. Then ensure we call
     * PLUGIN_EVENT_MAP_CASCADE so that the events defined in the DrawingTest
     * parent class are also handled.
     */
    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::X11Event, onX11Event, FB::PluginWindow)
        PLUGIN_EVENT_MAP_CASCADE(DrawingTest)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onX11Event(FB::X11Event *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/

    bool onGdkExpose(GtkWidget *canvas);

protected:
    /**
     * Function that changes the colour to draw and invalidates the screen
     */
    void update();

private:
    /**
     * This is the red component of the colour we paint the plugin. It'll
     * get incrememented in the update function every 40ms
     */
    unsigned int m_red;

    /**
     * The timer that calls the update function every 40ms
     */
    FB::TimerPtr m_timer;

    /**
     * Window object that we'll store so we can invalidate it in the update thread
     */
    FB::PluginWindow* m_window;
};
