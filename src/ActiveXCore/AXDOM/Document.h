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
#ifndef H_AXDOM_DOCUMENT
#define H_AXDOM_DOCUMENT

#include <string>
#include "win_common.h"
#include <atlctl.h>
#include "JSObject.h"
#include "Element.h"
#include "DOM/Document.h"
#include "DOM/Element.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning( disable : 4250 )
#endif
namespace FB { namespace ActiveX {
    namespace AXDOM {

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  Document
        ///
        /// @brief  ActiveX specific implementation of DOM::Document
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class Document : public virtual FB::ActiveX::AXDOM::Element, public virtual FB::DOM::Document
        {
        public:
            Document(const FB::JSObjectPtr &element, IWebBrowser2 *web);
            virtual ~Document();

        public:
            virtual FB::DOM::WindowPtr getWindow() const;
            virtual FB::DOM::ElementPtr getElementById(const std::string& elem_id) const;
            virtual std::vector<FB::DOM::ElementPtr> getElementsByTagName(const std::string& tagName) const;
			virtual FB::DOM::ElementPtr createElement(const std::string &name) const;
        protected:
            CComQIPtr<IWebBrowser> m_webBrowser;
            CComQIPtr<IHTMLDocument2> m_htmlDoc;
        };

    };
} }
#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // H_AXDOM_DOCUMENT

