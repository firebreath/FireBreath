/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#ifdef FB_WIN32
#define _WIN32_DCOM
#endif

#include "NpapiTypes.h"
#ifdef FB_WIN32
#include "atlbase.h"
#endif
#include "BasicMediaPlayer.h"
#ifdef FB_WIN32
#include "Win/PluginWindowWin.h"
#endif
#ifdef FB_X11
//#include "X11/PluginWindowX11.h"
#endif
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
  : m_player()
  , m_window(0)
{
}

BasicMediaPlayerPlugin::~BasicMediaPlayerPlugin()
{
}

FB::JSAPIPtr BasicMediaPlayerPlugin::createJSAPI()
{
    // m_host is the BrowserHost
    m_player = BasicMediaPlayerPtr(new BasicMediaPlayer(m_host));
    m_player->setWindow(m_window);
    return m_player;
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
    m_window = win;
    if(m_player)
        m_player->setWindow(win);
    return true;
}

bool BasicMediaPlayerPlugin::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* win)
{
    m_player->setWindow(NULL);
    return true;
}

