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
        JSAPI_DOMNode(const JSObject element) : m_element(element) { }
        JSAPI_DOMNode(const JSAPI_DOMNode &rhs) : m_element(rhs.m_element) { }
        virtual ~JSAPI_DOMNode() { }

        JSObject getJSObject() { return m_element; }

    public:
        template <class T>
        T callMethod(std::string name, VariantList args)
        {
            FB::variant tmp = m_element->Invoke(name, args);
            return tmp.convert_cast<T>();
        }

        template <class T>
        T getProperty(std::string name)
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

        JSAPI_DOMNode getNode(std::string name);

        JSAPI_DOMNode getNode(int idx);

        void setProperty(std::string name, variant val);

        void setProperty(int idx, variant val);

    protected:
        JSObject m_element;
    };

};

#endif // H_FB_JSAPI_DOMNODE
