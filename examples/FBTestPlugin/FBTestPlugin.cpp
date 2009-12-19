/**********************************************************\ 

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object 
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include "NpapiTypes.h"
#include "atlbase.h"
#include "FBTestPluginAPI.h"

#include "FBTestPlugin.h"

void FBTestPlugin::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void FBTestPlugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


FBTestPlugin::FBTestPlugin()
{
}

FBTestPlugin::~FBTestPlugin()
{
}

FB::JSAPI* FBTestPlugin::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new FBTestPluginAPI(m_host);
}

bool FBTestPlugin::onMouseDown(FB::MouseDownEvent *evt)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool FBTestPlugin::onMouseUp(FB::MouseUpEvent *evt)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool FBTestPlugin::onMouseMove(FB::MouseMoveEvent *evt)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
