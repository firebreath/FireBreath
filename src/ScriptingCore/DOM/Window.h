/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_DOM_WINDOW
#define H_FB_DOM_WINDOW

#include <string>
#include <boost/shared_ptr.hpp>
#include "Node.h"
#include "Document.h"

namespace FB { namespace DOM {

    class JSObject;
    /**
     * Window
     *
     * Provides a wrapper around a JSObject * that represents a DOM element
     **/
    class Window;
    typedef boost::shared_ptr<Window> WindowPtr;
    class Window : public Node
    {
    public:
        Window(const JSObjectPtr& element);
        virtual ~Window();
        WindowPtr window() { return boost::dynamic_pointer_cast<Window>(node()); }
        static WindowPtr create(FB::JSObjectPtr &api) { return api->host->_createWindow(api); }

    public:
        virtual DocumentPtr getDocument();
        virtual void alert(const std::string& str);
        virtual void alert(const std::wstring& str);
        virtual FB::JSObjectPtr createArray();
        virtual FB::JSObjectPtr createMap();
        virtual std::string getLocation();
    };

} };

#endif // H_FB_DOM_WINDOW
