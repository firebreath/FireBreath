#include "../DrawingTest.h"

#include <Win/PluginWindowWin.h>
#include <PluginEvents/DrawingEvents.h>
#include <Windows.h>
#include <boost/bind.hpp>
#include <Timer.h>

class WinDrawingTest : public DrawingTest
{
public:
    WinDrawingTest();

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
    void update();

private:
    unsigned int m_red;

    FB::TimerPtr m_timer;
};
