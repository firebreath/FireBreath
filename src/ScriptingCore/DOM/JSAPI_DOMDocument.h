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

#ifndef H_FB_JSAPI_DOMDOCUMENT
#define H_FB_JSAPI_DOMDOCUMENT

#include <string>
#include "BrowserObjectAPI.h"
#include "JSAPI_DOMElement.h"

namespace FB {

    class JSAPI_DOMWindow;
    /**
     * JSAPI_DOMElement
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class JSAPI_DOMDocument : public JSAPI_DOMElement
    {
    public:
        JSAPI_DOMDocument(const JSObject element);
        JSAPI_DOMDocument(const JSAPI_DOMDocument &rhs);
        JSAPI_DOMDocument(const JSAPI_DOMNode &rhs);
        virtual ~JSAPI_DOMDocument();

    public:
        JSAPI_DOMWindow getWindow();
        JSAPI_DOMElement getElementById(std::string id);
    };

};

#endif // H_FB_JSAPI_DOMDOCUMENT