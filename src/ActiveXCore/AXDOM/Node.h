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
#ifndef H_AXDOM_NODE
#define H_AXDOM_NODE

#include <string>
#include "win_common.h"
#include <atlctl.h>
#include "IDispatchAPI.h"
#include "JSObject.h"
#include "DOM/Node.h"

namespace FB { namespace ActiveX {
    namespace AXDOM {
        class Node;
        typedef boost::shared_ptr<Node> NodePtr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  Node
        ///
        /// @brief  Provides an ActiveX specific implementation of DOM::Node
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class Node : public virtual FB::DOM::Node
        {
        public:
            Node(const FB::JSObjectPtr& element, IWebBrowser *web);
            virtual ~Node();

        public:
			virtual FB::DOM::NodePtr appendChild(FB::DOM::NodePtr node);
        protected:
            CComQIPtr<IHTMLDOMNode> m_axNode;
            CComPtr<IWebBrowser> m_webBrowser;
        };

    };
} }
#endif // H_AXDOM_NODE

