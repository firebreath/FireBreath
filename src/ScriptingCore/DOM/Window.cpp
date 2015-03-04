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

#include "Deferred.h"
#include "Node.h"
#include "Element.h"
#include "Document.h"

#include "Window.h"

using namespace FB::DOM;
using FB::Promise;

Window::Window(const FB::JSObjectPtr& element) : Node(element)
{
}

Window::~Window()
{
}

Promise<DocumentPtr> Window::getDocument() const
{
    return getProperty<JSObjectPtr>("document").then<DocumentPtr>([=](JSObjectPtr api) {
        return Document::create(api);
    });
}

void Window::alert(const std::wstring& str) const
{
    alert(FB::wstring_to_utf8(str));
}

void Window::alert(std::string str) const
{
    callMethod<variant>("alert", VariantList{ str });
}

Promise<FB::JSObjectPtr> Window::createArray() const
{
    return callMethod<JSObjectPtr>("Array", FB::VariantList());
}

Promise<FB::JSObjectPtr> Window::createMap() const
{
    auto arrDfd = this->callMethod<JSObjectPtr>("Object", FB::VariantList());
    return arrDfd;
}

Promise<std::string> Window::getLocation() const
{
    return getNode("location").thenPipe<std::string>([=](NodePtr loc) {
        return loc->getProperty<std::string>("href");
    });
}

