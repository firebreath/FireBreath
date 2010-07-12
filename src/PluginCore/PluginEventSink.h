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

#ifndef H_FB_PLUGINEVENTSINK
#define H_FB_PLUGINEVENTSINK

#include "PluginEventSource.h"
#include <string>

namespace FB {

    class PluginEvent;
    /**
     * PluginWindow
     *
     * This is the base class for the main plugin class of a user-defined Plugin
     **/
    class PluginEventSink
    {
    public:
        PluginEventSink() { };
        virtual ~PluginEventSink() { };

    public:
        virtual bool HandleEvent(PluginEvent *, PluginEventSource *) = 0;
    };
};

#define BEGIN_PLUGIN_EVENT_MAP() virtual bool HandleEvent(FB::PluginEvent *evt, FB::PluginEventSource *win) { \
                                          if (0) { }

#define EVENTTYPE_CASE(eventType, methodName, winType) else if (evt->validType<eventType>()) { \
                                                return methodName(evt->get<eventType>(), win->get_as<winType>()); }

#define END_PLUGIN_EVENT_MAP() return false; }

#endif
