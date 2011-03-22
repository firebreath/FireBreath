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
#ifndef H_BASICMEDIAPLAYERPLUGINMAC
#define H_BASICMEDIAPLAYERPLUGINMAC

#include "PluginEvents/MacEventCarbon.h"
#include "PluginEvents/MacEventCocoa.h"

#include "../BasicMediaPlayerPlugin.h"

class BasicMediaPlayerPluginMac : public BasicMediaPlayerPlugin
{
public:
    BasicMediaPlayerPluginMac();
    virtual ~BasicMediaPlayerPluginMac();

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MacEventCocoa, onMacEventCocoa, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MacEventCarbon, onMacEventCarbon, FB::PluginWindow)
		else return BasicMediaPlayerPlugin::HandleEvent(evt, src);
    END_PLUGIN_EVENT_MAP()

    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow*);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow*);
    virtual bool onMacEventCarbon(FB::MacEventCarbon *evt, FB::PluginWindow*);
    virtual bool onMacEventCocoa(FB::MacEventCocoa *evt, FB::PluginWindow*);

protected:
#ifndef NP_NO_QUICKDRAW
    void DrawQuickDraw(CGrafPtr port, const FB::Rect& bounds, const FB::Rect& clip);
#endif
    void DrawCoreGraphics(CGContextRef context, const FB::Rect& bounds, const FB::Rect& clip);
    void DrawNSGraphicsContext(void *context, const FB::Rect& bounds, const FB::Rect& clip);

private:
    void* m_layer;
};

#endif

