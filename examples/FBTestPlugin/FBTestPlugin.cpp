/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include <sstream>
#include "NpapiTypes.h"
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

FB::JSAPIPtr FBTestPlugin::createJSAPI()
{
    // m_host is the BrowserHost
    return FB::JSAPIPtr(new FBTestPluginAPI(FB::ptr_cast<FBTestPlugin>(shared_ptr()), m_host));
}

bool FBTestPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*)
{
    std::ostringstream oStr;
    oStr << "Mouse down at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*)
{
    std::ostringstream oStr;
    oStr << "Mouse up at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*)
{
    std::ostringstream oStr;
    oStr << "Mouse move at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}
