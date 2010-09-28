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

#include "Element.h"

using namespace FB::DOM;

Element::Element(const FB::JSObjectPtr& element) : Node(element)
{
}

Element::~Element()
{
}

std::string Element::getInnerHTML()
{
    return getProperty<std::string>("innerHTML");
}
void Element::setInnerHTML(const std::string& html)
{
    setProperty("innerHTML", html);
}

int Element::getWidth()
{
    return getProperty<int>("width");
}
void Element::setWidth(int width)
{
    setProperty("width", width);
}

int Element::getHeight()
{
    return getProperty<int>("height");
}
void Element::setHeight(int height)
{
    setProperty("height", height);
}

int Element::getChildNodeCount()
{
    return getNode("childNodes")->getProperty<int>("length");
}

ElementPtr Element::getChildNode(int idx)
{
    ElementPtr retVal(getElement("childNodes")->getElement(idx));
    return retVal;
}

ElementPtr Element::getParentNode()
{
    ElementPtr retVal(getElement("parentNode"));
    return retVal;
}
