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

ElementImpl::ElementImpl(const FB::JSObject& element) : NodeImpl(element)
{
}

ElementImpl::~ElementImpl()
{
}

std::string ElementImpl::getInnerHTML()
{
    return getProperty<std::string>("innerHTML");
}
void ElementImpl::setInnerHTML(const std::string& html)
{
    setProperty("innerHTML", html);
}

int ElementImpl::getWidth()
{
    return getProperty<int>("width");
}
void ElementImpl::setWidth(int width)
{
    setProperty("width", width);
}

int ElementImpl::getHeight()
{
    return getProperty<int>("height");
}
void ElementImpl::setHeight(int height)
{
    setProperty("height", height);
}

int ElementImpl::getChildNodeCount()
{
    return getNode("childNodes")->getProperty<int>("length");
}

Element ElementImpl::getChildNode(int idx)
{
    Element retVal(getElement("childNodes")->getElement(idx));
    return retVal;
}

Element ElementImpl::getParentNode()
{
    Element retVal(getElement("parentNode"));
    return retVal;
}
