/**********************************************************\ 

  Auto-generated ${PLUGIN.ident}.cpp

  This file contains the auto-generated main plugin object 
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include "NpapiTypes.h"
#include "atlbase.h"
#include "MathAPI.h"

#include "${PLUGIN.ident}.h"

void ${PLUGIN.ident}::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void ${PLUGIN.ident}::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


${PLUGIN.ident}::${PLUGIN.ident}()
{
}

${PLUGIN.ident}::~${PLUGIN.ident}()
{
}

FB::JSAPI* ${PLUGIN.ident}::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new ${API.ident}API(m_host);
}

bool ${PLUGIN.ident}::onMouseDown(FB::MouseDownEvent *evt)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool ${PLUGIN.ident}::onMouseUp(FB::MouseUpEvent *evt)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool ${PLUGIN.ident}::onMouseMove(FB::MouseMoveEvent *evt)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
