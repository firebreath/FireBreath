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

#ifndef H_FB_JSAPI_DOMNODE
#define H_FB_JSAPI_DOMNODE

#include <string>
#include "BrowserObjectAPI.h"
namespace FB {
    /**
     * JSAPI_DOMNode
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM node
     **/
    class JSAPI_DOMNode
    {
    public:
        JSAPI_DOMNode(const JSObject& element) : m_element(element) { }
        JSAPI_DOMNode(const JSAPI_DOMNode &rhs) : m_element(rhs.m_element) { }
        virtual ~JSAPI_DOMNode() { }

        JSObject getJSObject() { return m_element; }

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

        JSAPI_DOMNode getNode(const std::wstring& name);
        JSAPI_DOMNode getNode(const std::string& name);

        JSAPI_DOMNode getNode(int idx);

        void setProperty(const std::wstring& name, const variant& val);
        void setProperty(const std::string& name, const variant& val);

        void setProperty(int idx, const variant& val);

    protected:
        JSObject m_element;
    };

};

#endif // H_FB_JSAPI_DOMNODE
