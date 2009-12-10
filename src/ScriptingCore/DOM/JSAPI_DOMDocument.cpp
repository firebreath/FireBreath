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

#include <boost/assign.hpp>
#include "JSAPI_DOMWindow.h"

#include "JSAPI_DOMDocument.h"

using boost::assign::list_of;

using namespace FB;

JSAPI_DOMDocument::JSAPI_DOMDocument(BrowserObjectAPI *element) : JSAPI_DOMElement(element)
{
}

JSAPI_DOMDocument::JSAPI_DOMDocument(JSAPI_DOMDocument& rhs) : JSAPI_DOMElement(rhs)
{
}

JSAPI_DOMDocument::~JSAPI_DOMDocument()
{
}

JSAPI_DOMWindow JSAPI_DOMDocument::getWindow()
{
    AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>("window");
    return JSAPI_DOMWindow(api);
}

JSAPI_DOMElement JSAPI_DOMDocument::getElementById(std::string id)
{
    AutoPtr<BrowserObjectAPI> api =
        callMethod<AutoPtr<BrowserObjectAPI>>("getElementById", FB::VariantList(list_of(id)));
    return JSAPI_DOMElement(api);
}