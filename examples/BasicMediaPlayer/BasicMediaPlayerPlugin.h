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
#ifndef H_BASICMEDIAPLAYERPLUGIN
#define H_BASICMEDIAPLAYERPLUGIN

#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginWindow.h"

#include "PluginCore.h"

class BasicMediaPlayer;

class BasicMediaPlayerPlugin : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    BasicMediaPlayerPlugin();
    virtual ~BasicMediaPlayerPlugin();

public:
    virtual FB::JSAPI* createJSAPI();
    virtual bool IsWindowless() { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*);
	virtual bool onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow*);
	virtual bool onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow*);

private:
    FB::AutoPtr<BasicMediaPlayer> m_player;
	FB::PluginWindow* m_window;
};

#endif
