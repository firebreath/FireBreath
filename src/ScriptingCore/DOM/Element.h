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
#ifndef H_FB_DOM_ELEMENT
#define H_FB_DOM_ELEMENT

#include <string>
#include "JSObject.h"
#include "Node.h"

namespace FB { namespace DOM {

    class Element;
    typedef boost::shared_ptr<Element> ElementPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Element
    ///
    /// @brief  DOM Element wrapper
    ///
    /// This class should not be created directly; instead, use the Element::create method
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Element : public virtual Node
    {
    public:
        Element(const FB::JSObjectPtr& element);
        virtual ~Element();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn ElementPtr Element::element()
        ///
        /// @brief  Gets a FB::DOM::ElementPtr pointing to the current object
        ///
        /// @return FB::DOM::ElementPtr point to the current object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ElementPtr element() { return boost::dynamic_pointer_cast<Element>(node()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static ElementPtr Element::create(const FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Element object from a JSObjectPtr representing a DOM object.  This
        ///         will probably throw an exception and definitely not work right if the DOM object
        ///         represented is not a Element
        ///
        /// @param  api The FB::JSObjectAPI object containing the element to wrap
        ///
        /// @return FB::DOM::ElementPtr to the created Element object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static ElementPtr create(const FB::JSObjectPtr &api) { return api->getHost()->_createElement(api); }

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string Element::getInnerHTML() const
        ///
        /// @brief  Gets the inner html for the element.
        ///
        /// @return The inner html.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getInnerHTML() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setInnerHTML(const std::string&) const
        ///
        /// @brief  Sets an inner html for the element
        ///
        /// @param  html    The html to set
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setInnerHTML(const std::string& html) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getWidth() const
        ///
        /// @brief  Gets the width property of the element.
        ///
        /// @return The width.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getWidth() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setWidth(int) const
        ///
        /// @brief  Sets the width property of the element.
        ///
        /// @param  width    The new width of the element.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setWidth(const int width) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getScrollWidth() const
        ///
        /// @brief  Gets the scroll width property (read-only) of the element.
        ///
        /// @return The scroll width.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getScrollWidth() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getHeight() const
        ///
        /// @brief  Gets the height property of the element.
        ///
        /// @return The height.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getHeight() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setHeight(int) const
        ///
        /// @brief  Sets the height property of the element
        ///
        /// @param  height    The new height of the element
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setHeight(const int height) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getScrollHeight() const
        ///
        /// @brief  Gets the scroll height property (read-only) of the element.
        ///
        /// @return The scroll height.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getScrollHeight() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getChildNodeCount() const
        ///
        /// @brief  Gets a child node count.
        ///
        /// @return The child node count.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getChildNodeCount() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getChildNode(int idx) const
        ///
        /// @brief  Gets a child node.
        ///
        /// @author Richard Bateman
        /// @date   10/15/2010
        ///
        /// @param  idx Zero-based index of the node to fetch.
        ///
        /// @return The child node.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getChildNode(const int idx) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getParentNode() const
        ///
        /// @brief  Gets the parent node of the current Element
        ///
        /// @return The parent node.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getParentNode() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getElement(const std::string& name) const
        ///
        /// @brief  Gets a child element of the specified name
        ///
        /// This is the rough equivilent of the following javascript command
        ///
        /// @code
        ///      // if name = child
        ///      var el2 = element.child;
        ///      // or
        ///      var el2 = element[name];
        /// @endcode
        ///
        /// @param  name    The name of the subelement.
        ///
        /// @return The child element.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElement(const std::string& name) const
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(name);
            ElementPtr retVal((api) ? new Element(api) : NULL);
            return retVal;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getElement(int idx) const
        ///
        /// @brief  Gets a child element of the specified name
        ///
        /// This is the rough equivilent of the following javascript command
        ///
        /// @code
        ///      var el2 = element[idx];
        /// @endcode
        ///
        /// @param  idx Zero-based index of the element to fetch.
        ///
        /// @return The child element.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElement(const int idx) const
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(idx);
            ElementPtr retVal((api) ? new Element(api) : NULL);
            return retVal;
        }

        virtual std::string getStringAttribute(const std::string& attr) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr getElementById(const std::string& elem_id) const
        ///
        /// @brief  Gets an element from the DOM with the specified id
        ///
        /// @param  elem_id Identifier for the element.
        ///
        /// @return The element by identifier.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElementById(const std::string& elem_id) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::vector<ElementPtr> getElementsByTagName(const std::string& tagName) const
        ///
        /// @brief  Gets a list of all elements in the document with the specified tag name
        ///
        /// @param  tagName Name of the tag.
        ///
        /// @return The elements by tag name.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::vector<ElementPtr> getElementsByTagName(const std::string& tagName) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual std::vector<ElementPtr> getElementsByTagName(const std::wstring& tagName) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::vector<ElementPtr> getElementsByTagName(const std::wstring& tagName) const;
    };

}; };

#endif // H_FB_DOM_ELEMENT

