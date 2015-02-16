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
#include "Element.h"

using namespace FB::DOM;

Element::Element(const FB::JSObjectPtr& element) : Node(element)
{
}

Element::~Element()
{
}

std::string Element::getInnerHTML() const
{
    return getProperty<std::string>("innerHTML");
}
void Element::setInnerHTML(std::string html) const
{
    setProperty("innerHTML", html);
}

int Element::getWidth() const
{
    return getProperty<int>("width");
}
void Element::setWidth(const int width) const
{
    setProperty("width", width);
}

int Element::getScrollWidth() const
{
    return getProperty<int>("scrollWidth");
}

int Element::getHeight() const
{
    return getProperty<int>("height");
}
void Element::setHeight(const int height) const
{
    setProperty("height", height);
}

int Element::getScrollHeight() const
{
    return getProperty<int>("scrollHeight");
}

int Element::getChildNodeCount() const
{
    return getNode("childNodes")->getProperty<int>("length");
}

ElementPtr Element::getChildNode(const int idx) const
{
    ElementPtr retVal(getElement("childNodes")->getElement(idx));
    return retVal;
}

ElementPtr Element::getParentNode() const
{
    ElementPtr retVal(getElement("parentNode"));
    return retVal;
}

ElementPtr Element::getElementById(std::string id) const
{
    JSObjectPtr api = callMethod<JSObjectPtr>("getElementById", VariantList{ id });
    return Element::create(api);
}

std::vector<ElementPtr> Element::getElementsByTagName(const std::wstring& tagName) const
{
    return getElementsByTagName(FB::wstring_to_utf8(tagName));
}

std::vector<ElementPtr> Element::getElementsByTagName(std::string tagName) const
{
    auto tagList = callMethod<std::vector<FB::JSObjectPtr> >("getElementsByTagName", VariantList{tagName});
    std::vector<ElementPtr> outList;

    for (auto tag : tagList)
    {
        outList.push_back(Element::create(tag));
    }
    return outList;
}

std::string FB::DOM::Element::getStringAttribute( std::string attr ) const
{
    return callMethod<std::string>("getAttribute", FB::VariantList{ attr });
}

