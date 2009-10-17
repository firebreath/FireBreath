/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_EVENTHANDLEROBJECT
#define H_FB_EVENTHANDLEROBJECT

#include "JSAPI.h"

namespace FB
{
    class EventHandlerObject : public FB::JSAPI
    {
    public:
        EventHandlerObject();
        virtual ~EventHandlerObject();

        virtual void *getEventId() { return NULL; }
        virtual void *getEventContext() { return NULL; }
    };
};
#endif