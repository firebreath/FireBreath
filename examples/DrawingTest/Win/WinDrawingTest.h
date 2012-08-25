#include "../DrawingTest.h"

#include <Win/PluginWindowWin.h>
#include <PluginEvents/DrawingEvents.h>
#include <Windows.h>
#include <boost/bind.hpp>
#include <Timer.h>

/**
 * Class WinDrawingTest extends from DrawingTest and implements all rendering
 * in a windows environment. It does this by listening for RefreshEvent's being raised
 * and then rendering to the Window we've been given.
 *
 * It also contains a timer that calls a function every 40ms (25 fps) that
 * changes the colour to render and then invalidates the window so that another
 * Refresh event is raised.
 */
class WinDrawingTest : public DrawingTest
{
public:
    WinDrawingTest();

    /* Create a plugin event map to get refresh events. Then ensure we calls
     * PLUGIN_EVENT_MAP_CASCADE so that the events defined in the DrawingTest
     * parent class are also handled.
     */
    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::RefreshEvent, onWindowRefresh, FB::PluginWindow)
        PLUGIN_EVENT_MAP_CASCADE(DrawingTest)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onWindowRefresh(FB::RefreshEvent *evt, FB::PluginWindow *);
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
