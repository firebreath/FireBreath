/**********************************************************\ 
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_EVENTS_X11EVENT
#define H_FB_EVENTS_X11EVENT

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
union _GdkEvent;
typedef union  _GdkEvent        GdkEvent;
#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  X11Event
    ///
    /// @brief  Generic X11 event
    ///
    /// @author Richard Bateman
    /// @date   10/15/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class X11Event : public PluginEvent
    {
    public:
        X11Event(GtkWidget *widget, GdkEvent *event) : m_widget(widget), m_event(event)
        {
        }

    public:
        GtkWidget *m_widget;
        GdkEvent *m_event;
    };
};

#endif

