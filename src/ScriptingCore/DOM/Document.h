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

    class Document;
    typedef boost::shared_ptr<Document> DocumentPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Document
    ///
    /// @brief  Abstraction for accessing and manipulating a DOM Document
    /// 		
    /// This class should not be created directly; instead, use the Document::create method
    /// 
    /// Most of the time you will want to call BrowserHost::getDOMDocument() to get the reference to
    /// the containing document
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Document : public Element
    {
    public:
        Document(const FB::JSObjectPtr &element);
        virtual ~Document();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn DocumentPtr Document::document()
        ///
        /// @brief  Gets a FB::DOM::DocumentPtr for the current object
        ///
        /// @return FB::DOM::DocumentPtr (alias to boost::shared_ptr<FB::DOM::Document>)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        DocumentPtr document() { return boost::dynamic_pointer_cast<Document>(node()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static DocumentPtr Document::create(FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Document object from a JSObjectPtr representing a DOM object.  This
        /// 		will probably throw an exception and definitely not work right if the DOM object
        /// 		represented is not a document
        ///
        /// @param  api The FB::JSObjectAPI object containing the document to wrap
        ///
        /// @return FB::DOM::DocumentPtr to the created Document object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static DocumentPtr create(FB::JSObjectPtr &api) { return api->host->_createDocument(api); }

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual WindowPtr Document::getWindow()
        ///
        /// @brief  Gets a DOM::WindowPtr for the window the document is contained in
        ///
        /// @return The window. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual WindowPtr getWindow();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Document::getElementById(const std::string& elem_id)
        ///
        /// @brief  Gets an element from the DOM with the specified id
        ///
        /// @param  elem_id Identifier for the element. 
        ///
        /// @return The element by identifier. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElementById(const std::string& elem_id);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::vector<ElementPtr> Document::getElementsByTagName(const std::string& tagName)
        ///
        /// @brief  Gets a list of all elements in the document with the specified tag name
        ///
        /// @param  tagName Name of the tag. 
        ///
        /// @return The elements by tag name. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::vector<ElementPtr> getElementsByTagName(const std::string& tagName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual std::vector<ElementPtr> Document::getElementsByTagName(const std::wstring& tagName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::vector<ElementPtr> getElementsByTagName(const std::wstring& tagName);
    };

}; };

#endif // H_FB_DOM_DOCUMENT