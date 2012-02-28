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

#include "JSObject.h"
#include "variant_list.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include "Node.h"

FB::DOM::NodePtr FB::DOM::Node::getNode(const std::wstring& name) const 
{
    return getNode(FB::wstring_to_utf8(name));
}
FB::DOM::NodePtr FB::DOM::Node::getNode(const std::string& name) const
{
    FB::JSObjectPtr api = getProperty<FB::JSObjectPtr>(name);
    return Node::create(api);
}

FB::DOM::NodePtr FB::DOM::Node::getNode(const int idx) const
{
    FB::JSObjectPtr api = getProperty<JSObjectPtr>(idx);
    return Node::create(api);
}

void FB::DOM::Node::setProperty(const std::wstring& name, const FB::variant& val) const
{
    setProperty(FB::wstring_to_utf8(name), val);
}
void FB::DOM::Node::setProperty(const std::string& name, const FB::variant& val) const
{
    m_element->SetProperty(name, val);
}

void FB::DOM::Node::setProperty(const int idx, const FB::variant& val) const
{
    m_element->SetProperty(idx, val);
}

FB::DOM::NodePtr FB::DOM::Node::appendChild(FB::DOM::NodePtr node)
{
	FB::JSObjectPtr result = callMethod<FB::JSObjectPtr>("appendChild", FB::variant_list_of(node->getJSObject()));
	return Node::create(result);
}
