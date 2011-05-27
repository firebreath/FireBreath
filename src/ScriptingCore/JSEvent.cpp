/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 17, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "variant.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "JSEvent.h"

FB::VariantMap FB::CreateEvent( const FB::JSAPIPtr& api, const std::string& name, const FB::VariantMap &members, const FB::VariantList &arguments )
{
    FB::VariantMap event;
    event["type"] = name;
    event["target"] = api;
    event["currentTarget"] = api;
    event["eventPhase"] = 2; // we're always AT_TARGET
    event["bubbles"] = false;
    event["cancelable"] = false; // Events in firebreath are async; not cancelable
    event["namespaceURI"] = name;
    //event["timeStamp"] = FB::FBDateString();

    // Add the custom members
    event.insert(members.begin(), members.end());
    event["arguments"] = arguments;
    return event;
}

FB::VariantMap FB::CreateEvent( const FB::JSAPIPtr& api, const std::string& name, const FB::VariantMap &members )
{
    return FB::CreateEvent(api, name, members, FB::VariantList());
}

FB::VariantMap FB::CreateEvent( const FB::JSAPIPtr& api, const std::string& name, const VariantList &arguments )
{
    return FB::CreateEvent(api, name, FB::VariantMap(), arguments);
}

