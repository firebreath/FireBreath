/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "variant_list.h"
#include "JSAPI_DOMWindow.h"

#include "JSAPI_DOMDocument.h"

using namespace FB;

JSAPI_DOMDocument::JSAPI_DOMDocument(const JSObject element) : JSAPI_DOMElement(element)
{
}

JSAPI_DOMDocument::JSAPI_DOMDocument(const JSAPI_DOMDocument& rhs) : JSAPI_DOMElement(rhs)
{
}

JSAPI_DOMDocument::~JSAPI_DOMDocument()
{
}

JSAPI_DOMWindow JSAPI_DOMDocument::getWindow()
{
    JSObject api = getProperty<JSObject>("window");
    JSAPI_DOMWindow retVal(api);
    return retVal;
}

JSAPI_DOMElement JSAPI_DOMDocument::getElementById(std::string id)
{
    JSObject api =
        callMethod<JSObject>("getElementById", FB::VariantList(variant_list_of(id)));
    JSAPI_DOMElement retVal(api);
    return retVal;
}