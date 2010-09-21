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

#ifndef H_FB_DOM_ELEMENT
#define H_FB_DOM_ELEMENT

#include <string>
#include "BrowserObjectAPI.h"
#include "Node.h"

namespace FB { namespace DOM {

    /**
     * ElementImpl (used as Element, a shared_ptr)
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class ElementImpl;
    typedef boost::shared_ptr<ElementImpl> Element;
    class ElementImpl : public NodeImpl
    {
    public:
        ElementImpl(const FB::JSObject& element);

        virtual ~ElementImpl();
        Element element() { return boost::dynamic_pointer_cast<ElementImpl>(node()); }

    public:
        virtual std::string getInnerHTML();
        virtual void setInnerHTML(const std::string&);

        virtual int getWidth();
        virtual void setWidth(int);
        virtual int getHeight();
        virtual void setHeight(int);

        virtual int getChildNodeCount();
        virtual Element getChildNode(int idx);
        virtual Element getParentNode();

        virtual Element getElement(const std::string& name)
        {
            JSObject api = getProperty<FB::JSObject>(name);
            Element retVal(new ElementImpl(api));
            return retVal;
        }

        virtual Element getElement(int idx)
        {
            JSObject api = getProperty<FB::JSObject>(idx);
            Element retVal(new ElementImpl(api));
            return retVal;
        }
    };

}; };

#endif // H_FB_DOM_ELEMENT