/**********************************************************\ 
 Original Author: Richard Bateman (taxilian)
 
 Created:    Jun 13, 2009
             License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2011 Facebook, Inc and Firebreath development team
 \**********************************************************/

#pragma once
#ifndef H_JSObjectRefAPI
#define H_JSObjectRefAPI

#include "JSObject.h"
#include "WebKitBrowserHost.h"
#include "ShareableReference.h"

namespace FB { namespace WebKit {
    
    FB_FORWARD_PTR(JSObjectRefAPI);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  NPObjectAPI
    ///
    /// @brief  Provides a FB::JSObject implementation that wraps a NPObject*
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSObjectRefAPI : public FB::JSObject
    {
    public:
        JSObjectRefAPI(JSObjectRef, const WebKitBrowserHostPtr&);
        virtual ~JSObjectRefAPI(void);
        
        void *getEventId() const { return (void*)getJSObjectRef(); }
        void *getEventContext() const {
            if (!m_browser.expired())
                return getHost()->getContextID();
            else
                return NULL;
        };
        JSObjectRef getJSObjectRef() const {
            JSObjectRefPtr ptr(obj.lock());
            if (ptr)
                return *ptr->getPtr();
            else 
                return NULL;
        }
        
        void getMemberNames(std::vector<std::string> &nameVector) const;
        size_t getMemberCount() const;
    public:
        virtual JSAPIPtr getJSAPI() const;
        void invalidate() { obj.reset(); m_browser.reset(); }
        bool isValid() { return !m_browser.expired(); }
        virtual bool supportsOptimizedCalls() const { return false; }
        virtual void callMultipleFunctions(const std::string& name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces) { }
        
    protected:
        WebKitBrowserHostPtr getHost() const {
            WebKitBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                throw std::bad_cast();
            }
            return ptr;
        }
        WebKitBrowserHostWeakPtr m_browser;
        JSObjectRefWeakPtr obj;
        JSContextRef m_jsContext;
        bool is_JSAPI;
        FB::JSAPIWeakPtr inner;
        
    public:
        bool HasMethod(const std::string& methodName) const;
        bool HasProperty(const std::string& propertyName) const;
        bool HasProperty(int idx) const;
        
        variant GetProperty(const std::string& propertyName);
        void SetProperty(const std::string& propertyName, const variant& value);
        void RemoveProperty(const std::string& propertyName);
        variant GetProperty(int idx);
        void SetProperty(int idx, const variant& value);
        void RemoveProperty(int idx);
        
        variant Invoke(const std::string& methodName, const std::vector<variant>& args);
        
        variant Construct(const std::vector<variant>& args);
    };
    
}; };

#endif

