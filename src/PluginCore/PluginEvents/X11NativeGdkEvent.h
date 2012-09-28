/**********************************************************\
File: X11NativeGdkEvent.h

Original Author: John Boyd (medusade)

Created: 5/4/2012
License: Dual license model; choose one of two:
         New BSD License
         http://www.opensource.org/licenses/bsd-license.php
         - or -
         GNU Lesser General Public License, version 2.1
         http://www.gnu.org/licenses/lgpl-2.1.html

Copyright  John Boyd (medusade), Firebreath development team
\**********************************************************/
#pragma once
#ifndef H_FB_EVENTS_X11NATIVEGDKEVENT
#define H_FB_EVENTS_X11NATIVEGDKEVENT

#include <gdk/gdk.h>

namespace FB {

///////////////////////////////////////////////////////////////////////
///  @class X11NativeGdkEvent
///
/// @author John Boyd (medusade)
///   @date 5/4/2012
///
///  @brief Class encapsulation for native Gdk event
///////////////////////////////////////////////////////////////////////
class X11NativeGdkEvent
{
public:
    GdkEvent m_gdkEvent;

    X11NativeGdkEvent
    (GdkWindow* window=0, GdkEventType type=GDK_NOTHING, gint8 send_event=TRUE)
    {
        m_gdkEvent.any.type = type;
        m_gdkEvent.any.window = window;
        m_gdkEvent.any.send_event = send_event;
    }
    bool SendEvent()
    {
        bool sent = false;
        if ((sent = (m_gdkEvent.any.type != GDK_NOTHING) && (m_gdkEvent.any.window != 0)))
        {
            gdk_threads_enter();
            gdk_event_put(&m_gdkEvent);
            gdk_threads_leave();
        }
        return sent;
    }
    inline operator GdkEvent* () const
    { return (GdkEvent*)(&m_gdkEvent); }
};

///////////////////////////////////////////////////////////////////////
///  @class X11NativeGdkEventExpose
///
/// @author John Boyd (medusade)
///   @date 5/4/2012
///
///  @brief Class encapsulation for native Gdk expose event
///////////////////////////////////////////////////////////////////////
class X11NativeGdkEventExpose: public X11NativeGdkEvent
{
public:
    typedef X11NativeGdkEvent Extends;

    X11NativeGdkEventExpose
    (GdkWindow* window=0, GdkRegion* region=0,
     int x=0, int y=0, int width=0, int height=0, int count=0,
     GdkEventType type=GDK_EXPOSE, gint8 send_event=TRUE)
    : Extends(window, type, send_event)
    {
        m_gdkEvent.expose.area.x = x;
        m_gdkEvent.expose.area.y = y;
        m_gdkEvent.expose.area.width = width;
        m_gdkEvent.expose.area.height = height;
        m_gdkEvent.expose.region = region;
        m_gdkEvent.expose.count = count;
    }
};

} // namespace FB 

#endif // H_FB_EVENTS_X11NATIVEGDKEVENT
