/**********************************************************\

  Auto-generated @{PLUGIN_ident}.cpp

  This file contains the auto-generated main plugin object
  implementation for the @{PLUGIN_name} project

\**********************************************************/

#include "NpapiTypes.h"
#include "@{PLUGIN_ident}API.h"

#include "@{PLUGIN_ident}.h"

void @{PLUGIN_ident}::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void @{PLUGIN_ident}::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


@{PLUGIN_ident}::@{PLUGIN_ident}()
{
}

@{PLUGIN_ident}::~@{PLUGIN_ident}()
{
}

FB::JSAPIPtr @{PLUGIN_ident}::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return FB::JSAPIPtr(new @{PLUGIN_ident}API(m_host));
}

bool @{PLUGIN_ident}::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool @{PLUGIN_ident}::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool @{PLUGIN_ident}::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool @{PLUGIN_ident}::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool @{PLUGIN_ident}::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}
