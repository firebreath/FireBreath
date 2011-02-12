/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_EVENTS_WINDOWATTACHEDEVENT
#define H_FB_EVENTS_WINDOWATTACHEDEVENT

#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  AttachedEvent
    ///
    /// @brief  Fired when a PluginEventSink is attached to a PluginEventSource (such as a PluginCore
    ///         derived plugin object being attached to a PluginWindow to get events)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class AttachedEvent : public PluginEvent
    {
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  DetachedEvent
    ///
    /// @brief  Fired when a PluginEventSink is detached from a PluginEventSource (such as a PluginCore
    ///         derived plugin object being detached from a PluginWindow to get events)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class DetachedEvent : public PluginEvent
    {
    };
};

#endif

