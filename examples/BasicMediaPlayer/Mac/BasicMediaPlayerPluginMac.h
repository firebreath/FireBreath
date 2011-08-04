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
#include "Mac/PluginWindowMac.h"
#include "Mac/PluginWindowMacCG.h"
#ifndef NP_NO_QUICKDRAW
#include "Mac/PluginWindowMacQD.h"
#endif

#include "../BasicMediaPlayerPlugin.h"

class BasicMediaPlayerPluginMac : public BasicMediaPlayerPlugin
{
public:
    BasicMediaPlayerPluginMac();
    virtual ~BasicMediaPlayerPluginMac();

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::CoreGraphicsDraw, onDrawCG, FB::PluginWindowMacCG)
#ifndef NP_NO_QUICKDRAW
        EVENTTYPE_CASE(FB::QuickDrawDraw, onDrawQD, FB::PluginWindowMacQD)
#endif
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindowMac)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindowMac)
        PLUGIN_EVENT_MAP_CASCADE(BasicMediaPlayerPlugin)
    END_PLUGIN_EVENT_MAP()

    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindowMac*);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindowMac*);
protected:
#ifndef NP_NO_QUICKDRAW
    bool onDrawQD(FB::QuickDrawDraw *evt, FB::PluginWindowMacQD*);
    void DrawQuickDraw(CGrafPtr port, const FB::Rect& bounds, const FB::Rect& clip);
#endif
    bool onDrawCG(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG*);
    void DrawCoreGraphics(CGContextRef context, const FB::Rect& bounds, const FB::Rect& clip);

    void DrawNSGraphicsContext(void *context, const FB::Rect& bounds, const FB::Rect& clip);

private:
    void* m_layer;
};

#endif

