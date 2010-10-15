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
#include "Window.h"

#include "Document.h"

using namespace FB::DOM;

Document::Document(const FB::JSObjectPtr& element) : Element(element)
{
}

Document::~Document()
{
}

WindowPtr Document::getWindow()
{
    JSObjectPtr api = getProperty<FB::JSObjectPtr>("window");
    return Window::create(api);
}

ElementPtr Document::getElementById(const std::string& id)
{
    JSObjectPtr api =
        callMethod<JSObjectPtr>("getElementById", FB::VariantList(variant_list_of(id)));
    return Element::create(api);
}

std::vector<ElementPtr> Document::getElementsByTagName(const std::wstring& tagName)
{
    return getElementsByTagName(FB::wstring_to_utf8(tagName));
}

std::vector<ElementPtr> Document::getElementsByTagName(const std::string& tagName)
{
    std::vector<FB::JSObjectPtr> tagList = callMethod<std::vector<FB::JSObjectPtr> >("getElementsByTagName", FB::variant_list_of(tagName));
    std::vector<FB::JSObjectPtr>::iterator it;
    std::vector<ElementPtr> outList;
    for (it = tagList.begin(); it != tagList.end(); it++)
    {
        outList.push_back(Element::create(*it));
    }
    return outList;
}
