#include "../DrawingTest.h"

#include <PluginWindowX11.h>
#include <PluginEvents/DrawingEvents.h>
#include <PluginEvents/X11Event.h>
#include <boost/bind.hpp>
#include <Timer.h>

class X11DrawingTest : public DrawingTest
{
public:
    X11DrawingTest();

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
    void update();

private:
    unsigned int m_red;

    FB::TimerPtr m_timer;
};
