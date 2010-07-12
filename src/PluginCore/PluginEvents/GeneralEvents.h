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

#ifndef H_FB_EVENTS_GENERALEVENTS
#define H_FB_EVENTS_GENERALEVENTS

#include "PluginEvent.h"

namespace FB {

    class TimerEvent : public PluginEvent
    {
    public:
        TimerEvent(unsigned int timer_id, void *opaquePointer)
            : timerId(timer_id), opaquePointer(opaquePointer)
        { }

    public:
        unsigned int timerId;
        void *opaquePointer;
    };
};

#endif