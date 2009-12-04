/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "EventHandlerObject.h"
#include "AsyncBrowserCall.h"

using namespace FB;

EventHandlerObject::EventHandlerObject(BrowserHostWrapper *h) :
    host(h)
{
}

EventHandlerObject::~EventHandlerObject(void)
{
}

void EventHandlerObject::InvokeAsync(std::string methodName, std::vector<variant>& args)
{
    FB::AsyncBrowserCall::CallMethod(this, methodName, args);
}