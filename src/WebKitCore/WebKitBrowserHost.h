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
#ifndef H_WEBKITBROWSERHOST
#define H_WEBKITBROWSERHOST

#include <boost/thread/recursive_mutex.hpp>
#include <objc/objc.h>
#include <objc/runtime.h>
#include "BrowserHost.h"
#include "ShareableReference.h"
#include <JavaScriptCore/JSObjectRef.h>

namespace FB { namespace WebKit {
    
    FB_FORWARD_PTR(WebKitBrowserHost);
    typedef boost::shared_ptr<FB::ShareableReference<JSObjectRef> > JSObjectRefPtr;
    typedef boost::weak_ptr<FB::ShareableReference<JSObjectRef> > JSObjectRefWeakPtr;
    class WebKitBrowserHost : public FB::BrowserHost
    {
    public:
        WebKitBrowserHost(JSContextRef jsContext, JSObjectRef window, const FB::BrowserHostPtr& parentHost);
        ~WebKitBrowserHost();
        
    public:
        // FB::BrowserHost required methods
        virtual void *getContextID() const;
        virtual JSContextRef getJSContext() const;
        
        void shutdown();
        
        virtual DOM::DocumentPtr getDOMDocument();
        virtual DOM::WindowPtr getDOMWindow();
        virtual DOM::ElementPtr getDOMElement();
        
        virtual FB::variant jsEval(const std::string &script);
        virtual void evaluateJavaScript(const std::string &script);
        virtual void DoDeferredRelease() const;
        virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const;

        virtual BrowserStreamPtr _createStream( const BrowserStreamRequest& req ) const;
        virtual BrowserStreamPtr _createUnsolicitedStream(const BrowserStreamRequest& req) const;

    public:
        // Implementation specific methods
        FB::variant getVariant(JSValueRef input);
        JSValueRef JSValueRefFromVariant(const FB::variant& var);
        
        JSObjectRefPtr ProtectJSObjectRef(JSObjectRef o);
        void ReleaseJSObjectRef(const JSObjectRefWeakPtr& oref);
        void deferred_release(JSObjectRef o) const;
    private:
        JSContextRef m_jsContext;
        JSObjectRef m_jsWindow;
        std::list<JSObjectRefPtr> m_jsObjectList;
        mutable std::list<JSObjectRef> m_deferredRelease;
        mutable boost::recursive_mutex m_deferMutex;
        mutable boost::recursive_mutex m_jsObjectMutex;
        mutable FB::BrowserHostPtr m_parentHost;
    };
    
}}

#endif // H_WEBKITBROWSERHOST