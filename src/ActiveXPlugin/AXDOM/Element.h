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

#pragma once
#ifndef H_AXDOM_ELEMENT
#define H_AXDOM_ELEMENT

#include "Win/win_common.h"
#include <atlctl.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include "IDispatchAPI.h"
#include "JSObject.h"
#include "DOM/Element.h"

namespace AXDOM {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Element
    ///
    /// @brief  ActiveX specific implementation of DOM::Element
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Element : public FB::DOM::Element
    {
    public:
        Element(const FB::JSObjectPtr& element, IWebBrowser *web);
        virtual ~Element();

    public:

        CComQIPtr<IHTMLElement> m_axElement;
        CComPtr<IWebBrowser> m_webBrowser;
    };

};

#endif // H_AXDOM_ELEMENT
