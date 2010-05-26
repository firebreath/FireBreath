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

#ifndef H_FB_JSAPI_DOMELEMENT
#define H_FB_JSAPI_DOMELEMENT

#include <string>
#include "BrowserObjectAPI.h"
#include "JSAPI_DOMNode.h"

namespace FB {

    /**
     * JSAPI_DOMElement
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class JSAPI_DOMElement : public JSAPI_DOMNode
    {
    public:
        JSAPI_DOMElement(const JSObject element);
        JSAPI_DOMElement(const JSAPI_DOMElement &rhs);

        virtual ~JSAPI_DOMElement();

    public:
        virtual std::string getInnerHTML();
        virtual void setInnerHTML(std::string);

        virtual int getWidth();
        virtual void setWidth(int);
        virtual int getHeight();
        virtual void setHeight(int);

        virtual int getChildNodeCount();
        virtual JSAPI_DOMElement getChildNode(int idx);
        virtual JSAPI_DOMElement getParentNode();

        JSAPI_DOMElement getElement(std::string name)
        {
            JSObject api = getProperty<JSObject>(name);
            JSAPI_DOMElement retVal(api);
            return retVal;
        }

        JSAPI_DOMElement getElement(int idx)
        {
            JSObject api = getProperty<JSObject>(idx);
            JSAPI_DOMElement retVal(api);
            return retVal;
        }
    };

};

#endif // H_FB_JSAPI_DOMELEMENT