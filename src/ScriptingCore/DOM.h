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

#ifndef H_FB_DOM
#define H_FB_DOM

#include <boost/shared_ptr.hpp>

namespace FB { namespace DOM {

    class WindowImpl;
    class DocumentImpl;
    class ElementImpl;
    class NodeImpl;

    typedef boost::shared_ptr<WindowImpl> Window;
    typedef boost::shared_ptr<DocumentImpl> Document;
    typedef boost::shared_ptr<ElementImpl> Element;
    typedef boost::shared_ptr<NodeImpl> Node;

}; };

#include "DOM/Node.h"
#include "DOM/Element.h"
#include "DOM/Document.h"
#include "DOM/Window.h"

#endif // H_FB_DOM