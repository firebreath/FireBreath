/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#define _WIN32_DCOM

#include "NpapiTypes.h"
#include "atlbase.h"
#include "BasicMediaPlayer.h"
#include "Win/PluginWindowWin.h"
#include "BasicMediaPlayerPlugin.h"

void BasicMediaPlayerPlugin::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void BasicMediaPlayerPlugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


BasicMediaPlayerPlugin::BasicMediaPlayerPlugin()
{
}

BasicMediaPlayerPlugin::~BasicMediaPlayerPlugin()
{
}

FB::JSAPI* BasicMediaPlayerPlugin::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    m_player = new BasicMediaPlayer(m_host);
    return m_player.ptr();
}

bool BasicMediaPlayerPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool BasicMediaPlayerPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool BasicMediaPlayerPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool BasicMediaPlayerPlugin::onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* win)
{
    m_player->setWindow(win);
    return true;
}

bool BasicMediaPlayerPlugin::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* win)
{
    m_player->setWindow(NULL);
    return true;
}
