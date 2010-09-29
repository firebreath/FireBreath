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
#include "JSObject.h"
#include "Node.h"

namespace FB { namespace DOM {

    /**
     * Element (used as ElementPtr, a shared_ptr)
     *
     * Provides a wrapper around a JSObject * that represents a DOM element
     **/
    class Element;
    typedef boost::shared_ptr<Element> ElementPtr;
    class Element : public Node
    {
    public:
        Element(const FB::JSObjectPtr& element);

        virtual ~Element();
        ElementPtr element() { return boost::dynamic_pointer_cast<Element>(node()); }
        static ElementPtr create(FB::JSObjectPtr &api) { return api->host->_createElement(api); }

    public:
        virtual std::string getInnerHTML();
        virtual void setInnerHTML(const std::string&);

        virtual int getWidth();
        virtual void setWidth(int);
        virtual int getHeight();
        virtual void setHeight(int);

        virtual int getChildNodeCount();
        virtual ElementPtr getChildNode(int idx);
        virtual ElementPtr getParentNode();

        virtual ElementPtr getElement(const std::string& name)
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(name);
            ElementPtr retVal(new Element(api));
            return retVal;
        }

        virtual ElementPtr getElement(int idx)
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(idx);
            ElementPtr retVal(new Element(api));
            return retVal;
        }
    };

}; };

#endif // H_FB_DOM_ELEMENT