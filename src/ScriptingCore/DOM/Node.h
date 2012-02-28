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
#ifndef H_FB_DOM_NODE
#define H_FB_DOM_NODE

#include <string>
#include <boost/enable_shared_from_this.hpp>
#include "JSObject.h"

namespace FB { namespace DOM {
    /**
     * Node (used as NodePtr, a shared_ptr)
     *
     * Provides a wrapper around a JSObject * that represents a DOM node
     **/
    class Node;
    typedef boost::shared_ptr<Node> NodePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Node
    ///
    /// @brief  DOM Node wrapper
    ///
    /// This class should not be created directly; instead, use the Node::create method
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Node : public boost::enable_shared_from_this<Node>
    {
    public:
        Node(const JSObjectPtr& element) : m_element(element) { }
        virtual ~Node() { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::JSObjectPtr getJSObject() const
        ///
        /// @brief  Returns the FB::JSObjectPtr that this object is wrapping
        ///
        /// @return The js object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::JSObjectPtr getJSObject() const { return m_element; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn NodePtr node()
        ///
        /// @brief  Gets the NodePtr for the current node
        ///
        /// @return NodePtr
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        NodePtr node() { return shared_from_this(); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static NodePtr create(const FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Node object from a JSObjectPtr representing a DOM node.  This
        ///         will probably throw an exception and definitely not work right if the js object
        ///         represented is not a node
        ///
        /// @param  api The FB::JSObjectAPI object containing the node to wrap
        ///
        /// @return FB::DOM::NodePtr to the created Node object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static NodePtr create(const FB::JSObjectPtr &api) { return api->getHost()->_createNode(api); }

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <class T> T callMethod(const std::string& name, const VariantList& args) const
        ///
        /// @brief  Templated function, calls a method on the node of the given name, casts the result to
        ///         type T, and returns it
        ///
        /// @code
        ///      double tmp = node.callMethod<double>("pow", FB::variant_list_of(3)(2));
        /// @endcode
        ///
        /// @param  name the name of the method to call
        /// @param  args a VariantList of arguments to pass ot the method
        ///
        /// @return result of calling the method cast as type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        T callMethod(const std::string& name, const VariantList& args) const
        {
            FB::variant tmp = m_element->Invoke(name, args);
            return tmp.convert_cast<T>();
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload template <class T> T callMethod(const std::wstring& name, const VariantList& args) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        T callMethod(const std::wstring& name, const VariantList& args) const
        {
            return callMethod<T>(FB::wstring_to_utf8(name), args);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasProperty(const std::string& propertyName) const
        ///
        /// @brief  Query if 'propertyName' is a valid property.
        ///
        /// @param  propertyName    Name of the property.
        ///
        /// @return true if property exists, false if not.
        /// @since 1.6.1
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool hasProperty(const std::string& propertyName) const {
            return m_element->HasProperty(propertyName);
        }

        /// @overload
        template <class T>
        T getProperty(const std::wstring& name) const
        {
            return getProperty<T>(FB::wstring_to_utf8(name));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <class T> T getProperty(const std::string& name) const
        ///
        /// @brief  Templated function, gets the specified prooerty and returns it cast as the specified
        ///         type
        ///
        /// @code
        ///     int width = docNode.getProperty<int>("offsetWdith");
        /// @endcode
        ///
        /// @param  name the name of the property to get
        ///
        /// @return value of the requested property cast as type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        T getProperty(const std::string& name) const
        {
            FB::variant tmp = m_element->GetProperty(name);
            return tmp.convert_cast<T>();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <class T> T getProperty(int idx) const
        ///
        /// @brief  Templated function, gets the specified prooerty by index and returns it cast as the
        ///         specified type
        ///
        /// @param  idx the index of the property to get
        ///
        /// @return result of calling the method cast as type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        T getProperty(const int idx) const
        {
            FB::variant tmp = m_element->GetProperty(idx);
            return tmp.convert_cast<T>();
        }

        /// @overload
        virtual NodePtr getNode(const std::wstring& name) const;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual NodePtr getNode(const std::string& name) const
        ///
        /// @brief  Gets a child node of the specified name
        ///
        /// @param  name the name of the child node to fetch
        ///
        /// @return child node
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual NodePtr getNode(const std::string& name) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual NodePtr getNode(int idx) const
        ///
        /// @brief  Gets a child node of the specified name by index
        ///
        /// @param  idx the index of the child node to fetch
        ///
        /// @return child node
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual NodePtr getNode(const int idx) const;

        /// @overload
        virtual void setProperty(const std::wstring& name, const variant& val) const;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void setProperty(const std::string& name, const variant& val) const
        ///
        /// @brief  Sets a property on the node to a given value
        ///
        /// @param  name the name of the property to set
        /// @param  val the new value for the property
        ///
        /// @return result of calling the method cast as type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setProperty(const std::string& name, const variant& val) const;

        /// @overload
        virtual void setProperty(const int idx, const variant& val) const;

		virtual NodePtr appendChild(NodePtr node);

    protected:
        JSObjectPtr m_element; // The JSObject that this object wraps
    };

}; };

#endif // H_FB_DOM_NODE

