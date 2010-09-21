/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_AXDOM_ELEMENT
#define H_AXDOM_ELEMENT

#include "Win/win_common.h"
#include <atlctl.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include "IDispatchAPI.h"
#include "BrowserObjectAPI.h"
#include "DOM/Element.h"

namespace AXDOM {

    /**
     * ElementImpl (used as Element, a shared_ptr)
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class ElementImpl : public FB::DOM::ElementImpl
    {
    public:
        ElementImpl(const FB::JSObject& element, IHTMLElement *el);
        virtual ~ElementImpl();

    public:
        virtual FB::DOM::Element getElement(const std::string& name)
        {
            IDispatchAPIPtr api(as_IDispatchAPI(getProperty<FB::JSObject>(name)));
            CComQIPtr<IHTMLElement> tmp(api->getIDispatch());
            if (tmp)
                return FB::DOM::Element(new ElementImpl(as_JSObject(api), tmp.p));
            else
                return FB::DOM::ElementImpl::getElement(name);
        }

        virtual FB::DOM::Element getElement(int idx)
        {
            return getElement(boost::lexical_cast<std::string>(idx));
        }

        CComQIPtr<IHTMLElement> m_axElement;
    };

};

#endif // H_AXDOM_ELEMENT