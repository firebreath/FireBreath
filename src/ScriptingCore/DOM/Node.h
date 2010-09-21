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

#ifndef H_FB_DOM_NODE
#define H_FB_DOM_NODE

#include <string>
#include <boost/enable_shared_from_this.hpp>
#include "BrowserObjectAPI.h"

namespace FB { namespace DOM {
    /**
     * NodeImpl (used as Node, a shared_ptr)
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM node
     **/
    class NodeImpl;
    typedef boost::shared_ptr<NodeImpl> Node;

    class NodeImpl : public boost::enable_shared_from_this<NodeImpl>
    {
    public:
        NodeImpl(const JSObject& element) : m_element(element) { }
        virtual ~NodeImpl() { }

        virtual FB::JSObject getJSObject() { return m_element; }
        Node node() { return shared_from_this(); }

    public:
        template <class T>
        T callMethod(const std::string& name, const VariantList& args)
        {
            FB::variant tmp = m_element->Invoke(name, args);
            return tmp.convert_cast<T>();
        }
        template <class T>
        T callMethod(const std::wstring& name, const VariantList& args)
        {
            return callMethod<T>(FB::wstring_to_utf8(name), args); 
        }
        template <class T>
        T getProperty(const std::wstring& name)
        {
            return getProperty<T>(FB::wstring_to_utf8(name));
        }
        template <class T>
        T getProperty(const std::string& name)
        {
            FB::variant tmp = m_element->GetProperty(name);
            return tmp.convert_cast<T>();
        }

        template <class T>
        T getProperty(int idx)
        {
            FB::variant tmp = m_element->GetProperty(idx);
            return tmp.convert_cast<T>();
        }

        virtual Node getNode(const std::wstring& name);
        virtual Node getNode(const std::string& name);

        virtual Node getNode(int idx);

        virtual void setProperty(const std::wstring& name, const variant& val);
        virtual void setProperty(const std::string& name, const variant& val);

        virtual void setProperty(int idx, const variant& val);

    protected:
        JSObject m_element;
    };

}; };

#endif // H_FB_DOM_NODE
