/**********************************************************\

  Auto-generated @{PLUGIN.ident}.cpp

  This file contains the auto-generated main plugin object
  implementation for the @{PLUGIN.name} project

\**********************************************************/

#include "NpapiTypes.h"
#include "atlbase.h"
#include "@{PLUGIN.ident}API.h"

#include "@{PLUGIN.ident}Plugin.h"

void @{PLUGIN.ident}Plugin::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void @{PLUGIN.ident}Plugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


@{PLUGIN.ident}Plugin::@{PLUGIN.ident}Plugin()
{
}

@{PLUGIN.ident}Plugin::~@{PLUGIN.ident}Plugin()
{
}

FB::JSAPI* @{PLUGIN.ident}Plugin::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new @{PLUGIN.ident}API(m_host);
}

bool @{PLUGIN.ident}Plugin::onMouseDown(FB::MouseDownEvent *evt)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool @{PLUGIN.ident}Plugin::onMouseUp(FB::MouseUpEvent *evt)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool @{PLUGIN.ident}Plugin::onMouseMove(FB::MouseMoveEvent *evt)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
