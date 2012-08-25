#include "../DrawingTest.h"

#include <PluginWindowMacCG.h>
#include <PluginEvents/DrawingEvents.h>
#include "PluginEvents/DrawingEvents.h"
#include <boost/bind.hpp>
#include <Timer.h>

/**
 * Class MacDrawingTest extends from DrawingTest and implements all rendering
 * in an Mac environment. It does this by listening for CoreGraphicsDraw Event's
 * being raised and then rendering to the Window we've been given.
 *
 * It also contains a timer that calls a function every 40ms (25 fps) that
 * changes the colour to render and then invalidates the window so that another
 * Draw event is raised.
 */
class MacDrawingTest : public DrawingTest
{
public:
    MacDrawingTest();

    /* Create a plugin event map to get core graphics drawing events. Then ensure we call
     * PLUGIN_EVENT_MAP_CASCADE so that the events defined in the DrawingTest
     * parent class are also handled.
     */
    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::CoreGraphicsDraw, onDrawCG, FB::PluginWindowMacCG)
        PLUGIN_EVENT_MAP_CASCADE(DrawingTest)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onDrawCG(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG*);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/

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
