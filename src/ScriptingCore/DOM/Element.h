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
    class Element : public Node
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
        /// @fn static ElementPtr Element::create(FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Element object from a JSObjectPtr representing a DOM object.  This
        /// 		will probably throw an exception and definitely not work right if the DOM object
        /// 		represented is not a Element
        /// 		
        /// @param  api The FB::JSObjectAPI object containing the element to wrap
        ///
        /// @return FB::DOM::ElementPtr to the created Element object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static ElementPtr create(FB::JSObjectPtr &api) { return api->host->_createElement(api); }

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string Element::getInnerHTML()
        ///
        /// @brief  Gets the inner html for the element. 
        ///
        /// @return The inner html. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getInnerHTML();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setInnerHTML(const std::string&)
        ///
        /// @brief  Sets an inner html for the element
        ///
        /// @param  html    The html to set
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setInnerHTML(const std::string& html);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getWidth()
        ///
        /// @brief  Gets the width property of the element
        ///
        /// @return The width. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getWidth();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setWidth(int)
        ///
        /// @brief  Sets the width property of the element. 
        ///
        /// @param  width    The new width of the element
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setWidth(int width);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getHeight()
        ///
        /// @brief  Gets the height property of the element. 
        ///
        /// @return The height. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getHeight();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Element::setHeight(int)
        ///
        /// @brief  Sets the height property of the element
        ///
        /// @param  height    The new height of the element
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setHeight(int height);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual int Element::getChildNodeCount()
        ///
        /// @brief  Gets a child node count. 
        ///
        /// @return The child node count. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getChildNodeCount();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getChildNode(int idx)
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
        virtual ElementPtr getChildNode(int idx);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getParentNode()
        ///
        /// @brief  Gets the parent node of the current Element
        ///
        /// @return The parent node. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getParentNode();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getElement(const std::string& name)
        ///
        /// @brief  Gets a child element of the specified name
        /// 		
        /// This is the rough equivilent of the following javascript command
        ///
        /// @code
        /// 	 // if name = child
        /// 	 var el2 = element.child;
        /// 	 // or
        /// 	 var el2 = element[name];
        /// @endcode
        /// 
        /// @param  name    The name of the subelement. 
        ///
        /// @return The child element. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElement(const std::string& name)
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(name);
            ElementPtr retVal(new Element(api));
            return retVal;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ElementPtr Element::getElement(int idx)
        ///
        /// @brief  Gets a child element of the specified name
        /// 
        /// This is the rough equivilent of the following javascript command
        ///
        /// @code
        /// 	 var el2 = element[idx];
        /// @endcode
        /// 		
        /// @param  idx Zero-based index of the element to fetch. 
        ///
        /// @return The child element. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ElementPtr getElement(int idx)
        {
            JSObjectPtr api = getProperty<FB::JSObjectPtr>(idx);
            ElementPtr retVal(new Element(api));
            return retVal;
        }
    };

}; };

#endif // H_FB_DOM_ELEMENT
