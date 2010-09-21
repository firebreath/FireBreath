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

#ifndef H_FB_DOM_DOCUMENT
#define H_FB_DOM_DOCUMENT

#include <string>
#include "BrowserObjectAPI.h"
#include "Element.h"

namespace FB { namespace DOM {

    /**
     * DocumentImpl - used as Document (which is a shared_ptr)
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM document
     **/
    class DocumentImpl;
    typedef boost::shared_ptr<DocumentImpl> Document;

    class DocumentImpl : public ElementImpl
    {
    public:
        DocumentImpl(const FB::JSObject &element);
        virtual ~DocumentImpl();
        Document document() { return boost::dynamic_pointer_cast<DocumentImpl>(node()); }

    public:
        virtual Window getWindow();
        virtual Element getElementById(const std::string& elem_id);
        virtual std::vector<Element> getElementsByTagName(const std::string& tagName);
        virtual std::vector<Element> getElementsByTagName(const std::wstring& tagName);
    };

}; };

#endif // H_FB_DOM_DOCUMENT