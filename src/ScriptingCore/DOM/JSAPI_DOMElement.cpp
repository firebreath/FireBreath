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

#include "JSAPI_DOMElement.h"

using namespace FB;

JSAPI_DOMElement::JSAPI_DOMElement(const JSObject element) : JSAPI_DOMNode(element)
{
}

JSAPI_DOMElement::JSAPI_DOMElement(const JSAPI_DOMElement& rhs) : JSAPI_DOMNode(rhs)
{
}

JSAPI_DOMElement::~JSAPI_DOMElement()
{
}

std::string JSAPI_DOMElement::getInnerHTML()
{
    return getProperty<std::string>("innerHTML");
}
void JSAPI_DOMElement::setInnerHTML(std::string html)
{
    setProperty("innerHTML", html);
}

int JSAPI_DOMElement::getWidth()
{
    return getProperty<int>("width");
}
void JSAPI_DOMElement::setWidth(int width)
{
    setProperty("width", width);
}

int JSAPI_DOMElement::getHeight()
{
    return getProperty<int>("height");
}
void JSAPI_DOMElement::setHeight(int height)
{
    setProperty("height", height);
}

int JSAPI_DOMElement::getChildNodeCount()
{
    return getNode("childNodes").getProperty<int>("length");
}

JSAPI_DOMElement JSAPI_DOMElement::getChildNode(int idx)
{
    JSAPI_DOMElement retVal(getElement("childNodes").getElement(idx));
    return retVal;
}

JSAPI_DOMElement JSAPI_DOMElement::getParentNode()
{
    JSAPI_DOMElement retVal(getElement("parentNode"));
    return retVal;
}