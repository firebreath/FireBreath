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

#include "variant.h"
#include "variant_list.h"
#include "Window.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include "Document.h"

using namespace FB::DOM;
using FB::Promise;

Document::Document(const FB::JSObjectPtr& element) : Node(element), Element(element)
{
}

Document::~Document()
{
}

Promise<WindowPtr> Document::getWindow() const
{
    return getProperty<FB::JSObjectPtr>("window").then<WindowPtr>([=](JSObjectPtr api) {
        return Window::create(api);
    });
}

Promise<ElementPtr> Document::getHead() const
{
    return getProperty<FB::JSObjectPtr>("head").then<ElementPtr>([=](JSObjectPtr api) {
        return Element::create(api);
    });
}

Promise<ElementPtr> Document::getBody() const
{
    return getProperty<FB::JSObjectPtr>("body").then<ElementPtr>([=](JSObjectPtr api) {
        return Element::create(api);
    });
}

Promise<ElementPtr> Document::createElement(const std::string &name) const
{
    return callMethod<FB::JSObjectPtr>("createElement", FB::VariantList{ name }).then<ElementPtr>([=](JSObjectPtr api) {
        return Element::create(api);
    });
}
