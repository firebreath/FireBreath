/**********************************************************\ 

  Auto-generated {{PLUGIN_ID}}.cpp

  This file contains the auto-generated main plugin object 
  implementation for the {{PLUGIN_NAME}} project

\**********************************************************/

#include "NpapiTypes.h"
#include "atlbase.h"
#include "MathAPI.h"

#include "{{PLUGIN_ID}}.h"

void {{PLUGIN_ID}}::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void {{PLUGIN_ID}}::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


{{PLUGIN_ID}}::{{PLUGIN_ID}}()
{
}

{{PLUGIN_ID}}::~{{PLUGIN_ID}}()
{
}

FB::JSAPI* {{PLUGIN_ID}}::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new {{API_NAME}}API(m_host);
}

bool {{PLUGIN_ID}}::onMouseDown(FB::MouseDownEvent *evt)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool {{PLUGIN_ID}}::onMouseUp(FB::MouseUpEvent *evt)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool {{PLUGIN_ID}}::onMouseMove(FB::MouseMoveEvent *evt)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
