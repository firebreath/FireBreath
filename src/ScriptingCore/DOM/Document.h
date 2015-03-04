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

    FB_FORWARD_PTR(Document);

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
    class Document : public virtual Element
    {
    public:
        Document(const FB::JSObjectPtr &element);
        virtual ~Document();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn DocumentPtr Document::document()
        ///
        /// @brief  Gets a FB::DOM::DocumentPtr for the current object
        ///
        /// @return FB::DOM::DocumentPtr (alias to std::shared_ptr<FB::DOM::Document>)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        DocumentPtr document() { return std::dynamic_pointer_cast<Document>(node()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static DocumentPtr Document::create(const FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Document object from a JSObjectPtr representing a DOM object.  This
        ///         will probably throw an exception and definitely not work right if the DOM object
        ///         represented is not a document
        ///
        /// @param  api The FB::JSObjectAPI object containing the document to wrap
        ///
        /// @return FB::DOM::DocumentPtr to the created Document object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static DocumentPtr create(const FB::JSObjectPtr &api) {
			if (!api) { return DocumentPtr(); }
			return api->getHost()->_createDocument(api);
		}

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual WindowPtr Document::getWindow() const
        ///
        /// @brief  Gets a DOM::WindowPtr for the window the document is contained in
        ///
        /// @return The window. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Promise<WindowPtr> getWindow() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Document::getHead() const
        ///
        /// @brief  Gets a DOM::ElementPtr for the document's head.
        ///
        /// @return The document's head.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Promise<ElementPtr> getHead() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Document::getBody() const
        ///
        /// @brief  Gets a DOM::ElementPtr for the document's body.
        ///
        /// @return The document's body.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Promise<ElementPtr> getBody() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Document::createElement(const std::string &name) const
        ///
        /// @brief  roughly equivalent to calling document.createElement(name) from javascript
        ///
        /// @return DOM::ElementPtr wrapping the newly created element
        ////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Promise<ElementPtr> createElement(const std::string &name) const;
	
	};

}; };

#endif // H_FB_DOM_DOCUMENT

