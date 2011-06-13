/**********************************************************\
 Original Author: Richard Bateman (taxilian)

 Created:    Jun 7, 2011
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2011 Facebook, Inc and Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_JSObjectRefOBJECT
#define H_JSObjectRefOBJECT

#include <boost/noncopyable.hpp>
#include "APITypes.h"
#include "JSAPI.h"
#include "WebKitBrowserHost.h"
#include "JSFunction.h"
#include "ShareableReference.h"

namespace FB { namespace WebKit {
    FB_FORWARD_PTR(JSObjectRefObject);
    class JSObjectRefObject : boost::noncopyable
    {
    public:
        static bool isJSAPI(JSContextRef jsContext, JSObjectRef o);

        JSObjectRef getJSObjectRef() {
            return m_obj;
        }
    protected:
        WebKitBrowserHostPtr getHost() {
            WebKitBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                throw std::bad_cast();
            }
            return ptr;
        }
        static JSClassDefinition JSObjectRefObjectClassDef;
        static JSClassRef JSObjectRefObjectClass;
        static int32_t JSObjectRefObjectUseCount;
        FB::JSAPIWeakPtr m_api;
        WebKitBrowserHostWeakPtr m_browser;
        bool m_valid;
        bool m_autoRelease;
        JSContextRef m_jsContext;
        JSObjectRef m_obj;
        boost::shared_ptr<FB::ShareableReference< JSObjectRefObject > > m_sharedRef;

    public:
        static JSObjectRef NewObject(const WebKitBrowserHostPtr& host, const FB::JSAPIWeakPtr& api, bool auto_release = false);

        void setAPI(const FB::JSAPIWeakPtr& api, const WebKitBrowserHostPtr& host) {
            m_api = api;
            m_browser = host;
        }
        FB::JSAPIPtr getAPI() const
        {
            FB::JSAPIPtr ptr(m_api.lock());
            if (!ptr)
                throw std::bad_cast();
            return ptr;
        }
        const boost::shared_ptr<FB::ShareableReference< JSObjectRefObject > > getWeakReference() { return m_sharedRef; }

        virtual ~JSObjectRefObject(void);

        bool isValid() {
            return m_valid && !m_api.expired() && !m_browser.expired();
        }

    private:
        JSObjectRefObject(JSContextRef ctx, JSObjectRef obj);

    protected:
        bool HasProperty(JSStringRef propertyName);
        JSValueRef GetProperty(JSStringRef propertyName, JSValueRef *exception);
        bool SetProperty(JSStringRef propertyName, JSValueRef value, JSValueRef *exception);
        bool DeleteProperty(JSStringRef propertyName, JSValueRef *exception);
        void GetPropertyNames(JSPropertyNameAccumulatorRef propertyNames);
        JSValueRef CallAsFunction(JSObjectRef thisObject, size_t argumentCount,
                                  const JSValueRef arguments[], JSValueRef* exception);
        JSObjectRef CallAsConstructor(size_t argumentCount, const JSValueRef arguments[],
                                             JSValueRef* exception);

    public:
        // Static methods referenced in the JSClassDefinition
        static void _Initialize(JSContextRef ctx, JSObjectRef object);
        static void _Finalize(JSObjectRef object);
        static bool _HasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
        static JSValueRef _GetProperty (JSContextRef ctx, JSObjectRef object,
                JSStringRef propertyName, JSValueRef *exception);
        static bool _SetProperty(JSContextRef ctx, JSObjectRef object,
                JSStringRef propertyName, JSValueRef value, JSValueRef *exception);
        static bool _DeleteProperty(JSContextRef ctx, JSObjectRef object,
                JSStringRef propertyName, JSValueRef *exception);
        static void _GetPropertyNames( JSContextRef ctx, JSObjectRef object,
                JSPropertyNameAccumulatorRef propertyNames);
        static JSValueRef _CallAsFunction(JSContextRef ctx, JSObjectRef function,
                JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                JSValueRef* exception);
        static JSObjectRef _CallAsConstructor(JSContextRef ctx, JSObjectRef constructor,
                size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    public:
        FB_FORWARD_PTR(JSO_addEventListener);
        FB_FORWARD_PTR(JSO_removeEventListener);
        friend class JSO_addEventListener;
        friend class JSO_removeEventListener;
        JSO_addEventListenerPtr m_addEventFunc;
        JSO_removeEventListenerPtr m_removeEventFunc;

        class JSO_addEventListener : public FB::JSFunction
        {
        public:
            JSO_addEventListener(JSObjectRefObject* ptr)
            : FB::JSFunction(FB::JSAPIPtr(), "addEventListener", SecurityScope_Public), obj(ptr) { }
            FB::variant exec(const std::vector<variant>& args);
        private:
            JSObjectRefObject* obj;
        };
        class JSO_removeEventListener : public FB::JSFunction
        {
        public:
            JSO_removeEventListener(JSObjectRefObject* ptr)
            : FB::JSFunction(FB::JSAPIPtr(), "removeEventListener", SecurityScope_Public), obj(ptr) { }
            FB::variant exec(const std::vector<variant>& args);
        private:
            JSObjectRefObject* obj;
        };
    };

}; };

#endif

