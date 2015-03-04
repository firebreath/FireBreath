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
#include "Deferred.h"
#include "Node.h"

using namespace FB::DOM;
using FB::Promise;

Promise<NodePtr> Node::getNode(const std::wstring& name) const 
{
    return getNode(FB::wstring_to_utf8(name));
}
Promise<NodePtr> Node::getNode(std::string name) const
{
    auto dfd = getProperty<FB::JSObjectPtr>(name);
    auto onDone = [=](FB::JSObjectPtr api) -> NodePtr {
        return Node::create(api);
    };
    auto next = dfd.then<NodePtr>( onDone );
    return next;
}

Promise<NodePtr> Node::getNode(const int idx) const
{
    return getProperty<FB::JSObjectPtr>(idx).then<NodePtr>( [=](FB::JSObjectPtr api) {
        return Node::create(api);
    } );
}

void Node::setProperty(const std::wstring& name, const FB::variant& val) const
{
    setProperty(FB::wstring_to_utf8(name), val);
}
void Node::setProperty(std::string name, const FB::variant& val) const
{
    m_element->SetProperty(name, val);
}

void Node::setProperty(const int idx, const FB::variant& val) const
{
    m_element->SetProperty(idx, val);
}

Promise<NodePtr> Node::appendChild(NodePtr node)
{
    return callMethod<FB::JSObjectPtr>("appendChild", FB::VariantList{ node->getJSObject() })
        .then<NodePtr>([=](JSObjectPtr api) {
        return Node::create(api);
    });
}
