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

#include "variant_list.h"
#include "Node.h"
#include "Element.h"
#include "Document.h"

#include "Window.h"

using namespace FB::DOM;

WindowImpl::WindowImpl(const FB::JSObject& element) : NodeImpl(element)
{
}

WindowImpl::~WindowImpl()
{
}

Document WindowImpl::getDocument()
{
    JSObject api = getProperty<JSObject>("document");
    return Document(new DocumentImpl(api));
}

void WindowImpl::alert(const std::string& str)
{
    callMethod<void>("alert", variant_list_of(str));
}

Node WindowImpl::createArray()
{
    JSObject arr = this->callMethod<JSObject>("Array", FB::VariantList());
    return Node(new NodeImpl(arr));
}

Node WindowImpl::createMap()
{
    JSObject arr = this->callMethod<JSObject>("Object", FB::VariantList());
    return Node(new NodeImpl(arr));
}
