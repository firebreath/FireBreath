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

Document::Document(const FB::JSObjectPtr& element) : Node(element), Element(element)
{
}

Document::~Document()
{
}

WindowPtr Document::getWindow() const
{
    JSObjectPtr api = getProperty<FB::JSObjectPtr>("window");
    return Window::create(api);
}

ElementPtr Document::getBody() const
{
    JSObjectPtr api = getProperty<FB::JSObjectPtr>("body");
    return Element::create(api);
}

FB::DOM::ElementPtr Document::createElement(const std::string &name) const
{
	JSObjectPtr api = callMethod<FB::JSObjectPtr>("createElement", FB::variant_list_of(name));
	return Element::create(api);
}
