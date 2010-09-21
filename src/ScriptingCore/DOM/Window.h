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

#ifndef H_FB_DOM_WINDOW
#define H_FB_DOM_WINDOW

#include <string>
#include <boost/shared_ptr.hpp>
#include "Node.h"
#include "Document.h"

namespace FB { namespace DOM {

    class BrowserObjectAPI;
    /**
     * Window
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class WindowImpl;
    typedef boost::shared_ptr<WindowImpl> Window;
    class WindowImpl : public NodeImpl
    {
    public:
        WindowImpl(const JSObject& element);
        virtual ~WindowImpl();
        Window window() { return boost::dynamic_pointer_cast<WindowImpl>(node()); }

    public:
        virtual Document getDocument();
        virtual void alert(const std::string& str);
        virtual Node createArray();
        virtual Node createMap();
    };

} };

#endif // H_FB_DOM_WINDOW
