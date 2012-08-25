#include "../DrawingTest.h"

#include <PluginWindowMacCG.h>
#include <PluginEvents/DrawingEvents.h>
#include "PluginEvents/DrawingEvents.h"
#include <boost/bind.hpp>
#include <Timer.h>

class MacDrawingTest : public DrawingTest
{
public:
    MacDrawingTest();

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
    void update();

private:
    unsigned int m_red;

    FB::TimerPtr m_timer;
};
