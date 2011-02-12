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
#ifndef H_FB_DOM
#define H_FB_DOM

#include <boost/shared_ptr.hpp>

namespace FB { 
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @namespace  FB::DOM 
///
/// @brief These are convenience objects used to access and manipulate DOM objects.  They should
///        never be created directly; instead, get the Window object or the Document object
///        from the BrowserHost or create it with DOM::Element::create(obj) (or similar)
///        
/// FB::DOM contains the following classes:
///   - FB::DOM::Node
///   - FB::DOM::Element
///   - FB::DOM::Document
///   - FB::DOM::Window
/// 
/// You should never instantate these classes directly.  Instead, use:
///   - FB::BrowserHost::getDOMWindow() to get the current document window
///   - FB::BrowserHost::getDOMDocument() to get the current document
///   - FB::DOM::Node::create(obj) to create a wrapper for a JS DOM Node object
///   - FB::DOM::Element::create(obj) to create a wrapper for a JS DOM Element object
///   - FB::DOM::Document::create(obj) to create a wrapper for a JS DOM Document
///   - FB::DOM::Window::create(obj) to create a wrapper for a JS DOM Window
///  
/// Never use a raw pointer to one of these classes.  Instead, use the following shared_ptr
/// typedefs:
///   - FB::DOM::NodePtr
///   - FB::DOM::ElementPtr
///   - FB::DOM::DocumentPtr
///   - FB::DOM::WindowPtr
////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace DOM {

    class Window;
    class Document;
    class Element;
    class Node;

    /// @brief  shared_ptr for a FB::DOM::Window
    typedef boost::shared_ptr<Window> WindowPtr;
    /// @brief  shared_ptr for a FB::DOM::Document
    typedef boost::shared_ptr<Document> DocumentPtr;
    /// @brief  shared_ptr for a FB::DOM::Element
    typedef boost::shared_ptr<Element> ElementPtr;
    /// @brief  shared_ptr for a FB::DOM::Node
    typedef boost::shared_ptr<Node> NodePtr;

}; };

#include "DOM/Node.h"
#include "DOM/Element.h"
#include "DOM/Document.h"
#include "DOM/Window.h"

#endif // H_FB_DOM

