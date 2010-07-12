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

#ifndef H_FB_EVENTS_WINDOWATTACHEDEVENT
#define H_FB_EVENTS_WINDOWATTACHEDEVENT

#include "PluginEvent.h"

namespace FB {

    class AttachedEvent : public PluginEvent
    {
    };

    class DetachedEvent : public PluginEvent
    {
    };
};

#endif