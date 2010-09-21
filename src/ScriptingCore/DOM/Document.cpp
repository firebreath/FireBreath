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

DocumentImpl::DocumentImpl(const FB::JSObject& element) : ElementImpl(element)
{
}

DocumentImpl::~DocumentImpl()
{
}

Window DocumentImpl::getWindow()
{
    JSObject api = getProperty<FB::JSObject>("window");
    Window retVal(new WindowImpl(api));
    return retVal;
}

Element DocumentImpl::getElementById(const std::string& id)
{
    JSObject api =
        callMethod<JSObject>("getElementById", FB::VariantList(variant_list_of(id)));
    Element retVal(new ElementImpl(api));
    return retVal;
}

std::vector<Element> DocumentImpl::getElementsByTagName(const std::wstring& tagName)
{
    return getElementsByTagName(FB::wstring_to_utf8(tagName));
}

std::vector<Element> DocumentImpl::getElementsByTagName(const std::string& tagName)
{
    std::vector<FB::JSObject> tagList = callMethod<std::vector<FB::JSObject>>("getElementsByTagName", FB::variant_list_of(tagName));
    std::vector<FB::JSObject>::iterator it;
    std::vector<Element> outList;
    for (it = tagList.begin(); it != tagList.end(); it++)
    {
        outList.push_back(FB::DOM::Element(new ElementImpl(*it)));
    }
    return outList;
}