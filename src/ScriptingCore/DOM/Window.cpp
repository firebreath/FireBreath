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

#include "variantDeferred.h"
#include "Node.h"
#include "Element.h"
#include "Document.h"

#include "Window.h"

using namespace FB::DOM;

Window::Window(const FB::JSObjectPtr& element) : Node(element)
{
}

Window::~Window()
{
}

DocumentPtr Window::getDocument() const
{
    JSObjectPtr api = getProperty<JSObjectPtr>("document");
    return Document::create(api);
}

void Window::alert(const std::wstring& str) const
{
    alert(FB::wstring_to_utf8(str));
}

void Window::alert(std::string str) const
{
    callMethod<void>("alert", VariantList{ str });
}

FB::JSObjectPtr Window::createArray() const
{
    JSObjectPtr arr = this->callMethod<JSObjectPtr>("Array", FB::VariantList());
    return arr;
}

FB::JSObjectPtr Window::createMap() const
{
    JSObjectPtr arr = this->callMethod<JSObjectPtr>("Object", FB::VariantList());
    return arr;
}

std::string Window::getLocation() const
{
    return getNode("location")->getProperty<std::string>("href");
}

