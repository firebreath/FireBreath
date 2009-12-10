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

JSAPI_DOMWindow::JSAPI_DOMWindow(BrowserObjectAPI *element) : JSAPI_DOMNode(element)
{
}

JSAPI_DOMWindow::JSAPI_DOMWindow(JSAPI_DOMWindow &rhs) : JSAPI_DOMNode(rhs)
{
}

JSAPI_DOMWindow::~JSAPI_DOMWindow()
{
}

JSAPI_DOMElement JSAPI_DOMWindow::getDocument()
{
    AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>("document");
    return JSAPI_DOMDocument(api.ptr());
}

void JSAPI_DOMWindow::alert(std::string str)
{
    callMethod<void>("alert", VariantList(list_of(str)));
}