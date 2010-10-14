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

#pragma once
#ifndef H_FB_DOM_DOCUMENT
#define H_FB_DOM_DOCUMENT

#include <string>
#include "JSObject.h"
#include "Element.h"

namespace FB { namespace DOM {

    /**
     * Document - used as DocumentPtr (which is a shared_ptr)
     *
     * Provides a wrapper around a JSObject * that represents a DOM document
     **/
    class Document;
    typedef boost::shared_ptr<Document> DocumentPtr;

    class Document : public Element
    {
    public:
        Document(const FB::JSObjectPtr &element);
        virtual ~Document();
        DocumentPtr document() { return boost::dynamic_pointer_cast<Document>(node()); }
        static DocumentPtr create(FB::JSObjectPtr &api) { return api->host->_createDocument(api); }

    public:
        virtual WindowPtr getWindow();
        virtual ElementPtr getElementById(const std::string& elem_id);
        virtual std::vector<ElementPtr> getElementsByTagName(const std::string& tagName);
        virtual std::vector<ElementPtr> getElementsByTagName(const std::wstring& tagName);
    };

}; };

#endif // H_FB_DOM_DOCUMENT