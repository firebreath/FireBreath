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

#ifndef H_FB_JSAPI_DOMWINDOW
#define H_FB_JSAPI_DOMWINDOW

#include <string>
#include "JSAPI_DOMNode.h"
#include "JSAPI_DOMElement.h"

namespace FB {

    class JSAPI_DOMDocument;
    class BrowserObjectAPI;
    /**
     * JSAPI_DOMWindow
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class JSAPI_DOMWindow : public JSAPI_DOMNode
    {
    public:
        JSAPI_DOMWindow(const JSObject element);
        JSAPI_DOMWindow(const JSAPI_DOMWindow &rhs);
        virtual ~JSAPI_DOMWindow();

    public:
        JSAPI_DOMElement getDocument();
        void alert(std::string str);
        JSAPI_DOMNode createArray();
        JSAPI_DOMNode createMap();
    };

};

#endif // H_FB_JSAPI_DOMWINDOW