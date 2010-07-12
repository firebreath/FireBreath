/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"

#include "JSAPI_DOMNode.h"

FB::JSAPI_DOMNode FB::JSAPI_DOMNode::getNode(std::string name)
{
    FB::JSObject api = getProperty<FB::JSObject>(name);
    FB::JSAPI_DOMNode retVal(api);
    return retVal;
}

FB::JSAPI_DOMNode FB::JSAPI_DOMNode::getNode(int idx)
{
    FB::JSObject api = getProperty<JSObject>(idx);
    FB::JSAPI_DOMNode retVal(api);
    return retVal;
}

void FB::JSAPI_DOMNode::setProperty(std::string name, FB::variant val)
{
    m_element->SetProperty(name, val);
}

void FB::JSAPI_DOMNode::setProperty(int idx, FB::variant val)
{
    m_element->SetProperty(idx, val);
}
