/**********************************************************\ 

  Auto-generated ${PLUGIN.ident}.cpp

  This file contains the auto-generated main plugin object 
  implementation for the ${PLUGIN.name} project

\**********************************************************/
#ifndef H_TEMPLATEPLUGIN
#define H_TEMPLATEPLUGIN

#include "PluginEvents/MouseEvents.h"

#include "PluginCore.h"

class ${PLUGIN.ident} : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    ${PLUGIN.ident}();
    virtual ~${PLUGIN.ident}();

public:
    virtual FB::JSAPI* createJSAPI();
    virtual bool IsWindowless() { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt);
    virtual bool onMouseUp(FB::MouseUpEvent *evt);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
};

#endif
