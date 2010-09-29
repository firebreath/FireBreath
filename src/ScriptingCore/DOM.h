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

namespace FB { namespace DOM {

    class Window;
    class Document;
    class Element;
    class Node;

    typedef boost::shared_ptr<Window> WindowPtr;
    typedef boost::shared_ptr<Document> DocumentPtr;
    typedef boost::shared_ptr<Element> ElementPtr;
    typedef boost::shared_ptr<Node> NodePtr;

}; };

#include "DOM/Node.h"
#include "DOM/Element.h"
#include "DOM/Document.h"
#include "DOM/Window.h"

#endif // H_FB_DOM