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

Window::Window(const FB::JSObject& element) : Node(element)
{
}

Window::~Window()
{
}

DocumentPtr Window::getDocument()
{
    JSObject api = getProperty<JSObject>("document");
    return Document::create(api);
}

void Window::alert(const std::wstring& str)
{
    alert(FB::wstring_to_utf8(str));
}

void Window::alert(const std::string& str)
{
    callMethod<void>("alert", variant_list_of(str));
}

NodePtr Window::createArray()
{
    JSObject arr = this->callMethod<JSObject>("Array", FB::VariantList());
    return Node::create(arr);
}

NodePtr Window::createMap()
{
    JSObject arr = this->callMethod<JSObject>("Object", FB::VariantList());
    return Node::create(arr);
}

std::string Window::getLocation()
{
    return getNode("location")->getProperty<std::string>("href");
}