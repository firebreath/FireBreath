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
        JSAPI_DOMElement(BrowserObjectAPI *element);
        JSAPI_DOMElement(JSAPI_DOMElement &rhs);
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
            AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>(name);
            return JSAPI_DOMElement(api.ptr());
        }

        JSAPI_DOMElement getElement(int idx)
        {
            AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>(idx);
            return JSAPI_DOMElement(api.ptr());
        }
    };

};

#endif // H_FB_JSAPI_DOMELEMENT