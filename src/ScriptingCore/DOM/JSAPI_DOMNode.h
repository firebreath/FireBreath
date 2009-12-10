/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
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
        JSAPI_DOMNode(BrowserObjectAPI *element) : m_element(element) { }
        JSAPI_DOMNode(JSAPI_DOMNode &rhs) : m_element(rhs.m_element) { }
        virtual ~JSAPI_DOMNode() { }

        BrowserObjectAPI *getJSAPI() { return m_element; }

    public:
        template <class T>
        T callMethod(std::string name, VariantList args)
        {
            return m_element->Invoke(name, args).convert_cast<T>();
        }

        template <class T>
        T getProperty(std::string name)
        {
            return m_element->GetProperty(name).convert_cast<T>();
        }

        template <class T>
        T getProperty(int idx)
        {
            return m_element->GetProperty(idx).convert_cast<T>();
        }

        JSAPI_DOMNode getNode(std::string name)
        {
            AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>(name);
            return JSAPI_DOMNode(api.ptr());
        }
        
        JSAPI_DOMNode getNode(int idx)
        {
            AutoPtr<BrowserObjectAPI> api = getProperty<AutoPtr<BrowserObjectAPI>>(idx);
            return JSAPI_DOMNode(api.ptr());
        }
        
        void setProperty(std::string name, variant val)
        {
            m_element->SetProperty(name, val);
        }

        void setProperty(int idx, variant val)
        {
            m_element->SetProperty(idx, val);
        }

    protected:
        AutoPtr<BrowserObjectAPI> m_element;
    };

};

#endif // H_FB_JSAPI_DOMNODE