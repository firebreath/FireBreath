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

#include "Node.h"

FB::DOM::Node FB::DOM::NodeImpl::getNode(const std::wstring& name)
{
    return getNode(FB::wstring_to_utf8(name));
}
FB::DOM::Node FB::DOM::NodeImpl::getNode(const std::string& name)
{
    FB::JSObject api = getProperty<FB::JSObject>(name);
    FB::DOM::Node retVal(new NodeImpl(api));
    return retVal;
}

FB::DOM::Node FB::DOM::NodeImpl::getNode(int idx)
{
    FB::JSObject api = getProperty<JSObject>(idx);
    FB::DOM::Node retVal(new NodeImpl(api));
    return retVal;
}

void FB::DOM::NodeImpl::setProperty(const std::wstring& name, const FB::variant& val)
{
    setProperty(FB::wstring_to_utf8(name), val);
}
void FB::DOM::NodeImpl::setProperty(const std::string& name, const FB::variant& val)
{
    m_element->SetProperty(name, val);
}

void FB::DOM::NodeImpl::setProperty(int idx, const FB::variant& val)
{
    m_element->SetProperty(idx, val);
}
