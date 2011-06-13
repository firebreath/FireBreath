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

#include <boost/scoped_array.hpp>
#include "JSValueUtils.h"
#include "WebKitBrowserHost.h"
#include "JSObjectRefAPI.h"
#include "DOM.h"
#include <cstring>

using namespace FB::WebKit;

namespace
{
    struct MethodCallReq
    {
        //FB::variant
    };
    
    template<class T>
    JSValueConverterMap::value_type makeConverterEntry()
    {
        return JSValueConverterMap::value_type(&typeid(T), select_JSValueRef_builder::select<T>());
    }
    
    JSValueConverterMap makeJSValueConverterMap()
    {
        JSValueConverterMap tdm;
        tdm.insert(makeConverterEntry<bool>());
        tdm.insert(makeConverterEntry<char>());
        tdm.insert(makeConverterEntry<unsigned char>());
        tdm.insert(makeConverterEntry<short>());
        tdm.insert(makeConverterEntry<unsigned short>());
        tdm.insert(makeConverterEntry<int>());
        tdm.insert(makeConverterEntry<unsigned int>());
        tdm.insert(makeConverterEntry<long>());
        tdm.insert(makeConverterEntry<unsigned long>());
        
#ifndef BOOST_NO_LONG_LONG
        tdm.insert(makeConverterEntry<long long>());
        tdm.insert(makeConverterEntry<unsigned long long>());
#endif
        
        tdm.insert(makeConverterEntry<float>());
        tdm.insert(makeConverterEntry<double>());
        
        tdm.insert(makeConverterEntry<std::string>());
        tdm.insert(makeConverterEntry<std::wstring>());
        
        tdm.insert(makeConverterEntry<FB::FBNull>());
        tdm.insert(makeConverterEntry<FB::FBVoid>());
        tdm.insert(makeConverterEntry<FB::VariantList>());
        tdm.insert(makeConverterEntry<FB::VariantMap>());
        tdm.insert(makeConverterEntry<FB::JSAPIPtr>());
        tdm.insert(makeConverterEntry<FB::JSAPIWeakPtr>());
        tdm.insert(makeConverterEntry<FB::JSObjectPtr>());
        
        return tdm;
    }
    
    const JSValueConverterMap& getJSValueConverterMap()
    {
        static const JSValueConverterMap tdm = makeJSValueConverterMap();
        return tdm;
    }
}


WebKitBrowserHost::WebKitBrowserHost(JSContextRef jsContext, JSObjectRef window, const FB::BrowserHostPtr& parentHost)
    : m_jsContext(jsContext), m_jsWindow(window), m_parentHost(parentHost)
{
}

WebKitBrowserHost::~WebKitBrowserHost()
{
}

FB::DOM::DocumentPtr WebKitBrowserHost::getDOMDocument() {
    try {
        FB::JSObjectPtr doc(jsEval("document").convert_cast<FB::JSObjectPtr>());
        if (doc)
            return FB::DOM::Document::create(doc);
    }
    catch (...) {
    }
    return FB::DOM::DocumentPtr();
};
FB::DOM::WindowPtr WebKitBrowserHost::getDOMWindow() {
    try {
        FB::JSObjectPtr win(getVariant(m_jsWindow).convert_cast<FB::JSObjectPtr>());
        if (win)
            return FB::DOM::Window::create(win);
    }
    catch (...) {
    }
    return FB::DOM::WindowPtr();
}
FB::DOM::ElementPtr WebKitBrowserHost::getDOMElement() {
    try {
        FB::JSObjectPtr body(jsEval("document.body").convert_cast<FB::JSObjectPtr>());
        if (body)
            return FB::DOM::Element::create(body);
    }
    catch (...) {
    }
    return FB::DOM::ElementPtr();
}

FB::variant WebKitBrowserHost::getVariant(JSValueRef input)
{
    if (input == NULL)
        return FB::FBNull();
    
    switch(JSValueGetType(m_jsContext, input)) {
        case kJSTypeUndefined:
            return FB::FBVoid();
        case kJSTypeNull:
            return FB::FBNull();
        case kJSTypeBoolean:
            return JSValueToBoolean(m_jsContext, input);
        case kJSTypeNumber: {
            JSStringRef strVal = JSValueToStringCopy(m_jsContext, input, NULL);
            size_t strSize = JSStringGetMaximumUTF8CStringSize(strVal);
            boost::scoped_array<char> buf(new char[strSize]);
            JSStringGetUTF8CString(strVal, buf.get(), strSize);
            JSStringRelease(strVal);
            if (memchr(buf.get(), '.', strSize) != NULL) {
                return boost::lexical_cast<double>(buf.get());
            } else if (memchr(buf.get(), '-', strSize) != NULL) {
                int64_t fullSize = boost::lexical_cast<int64_t>(buf.get());
                try {
                    return boost::numeric_cast<int32_t>(fullSize);
                }
                catch (const boost::bad_numeric_cast&) {
                    return fullSize;
                }
            } else {
                uint64_t fullSize = boost::lexical_cast<uint64_t>(buf.get());
                try {
                    return boost::numeric_cast<uint32_t>(fullSize);
                }
                catch (const boost::bad_numeric_cast&) {
                    return fullSize;
                }
            }
            break; // unreachable, but easier to read
        }
        case kJSTypeString: {
            JSStringRef strVal = JSValueToStringCopy(m_jsContext, input, NULL);
            size_t strSize = JSStringGetMaximumUTF8CStringSize(strVal);
            boost::scoped_array<char> buf(new char[strSize]);
            JSStringGetUTF8CString(strVal, buf.get(), strSize);
            JSStringRelease(strVal);
            return buf.get();
        }
        case kJSTypeObject: {
            JSValueRef except(NULL);
            JSObjectRef o(JSValueToObject(m_jsContext, input, &except));
            if (except != NULL) {
                JSStringRef strVal = JSValueToStringCopy(m_jsContext, except, NULL);
                size_t strSize = JSStringGetMaximumUTF8CStringSize(strVal);
                boost::scoped_array<char> buf(new char[strSize]);
                JSStringGetUTF8CString(strVal, buf.get(), strSize);
                JSStringRelease(strVal);
                throw FB::script_error(buf.get());
            } else if (o == NULL) {
                return FB::FBVoid();
            } else {
                return JSObjectRefAPIPtr(
                    new JSObjectRefAPI(o, boost::static_pointer_cast<WebKitBrowserHost>( shared_from_this() ))
                );
            }

        }
        default:
            return FB::FBVoid();
    }
    return FB::FBVoid();
}

JSValueRef WebKitBrowserHost::JSValueRefFromVariant(const FB::variant& var)
{
    const JSValueConverterMap& convertMap = getJSValueConverterMap();
    const std::type_info& type = var.get_type();
    
    JSValueConverterMap::const_iterator it = convertMap.find(&type);
    
    if (it == convertMap.end()) {
        // Unhandled type!
        return JSValueMakeUndefined(getJSContext());
    }
    return (it->second)(boost::static_pointer_cast<WebKitBrowserHost>(shared_from_this()), var);    
}

void WebKitBrowserHost::shutdown()
{
    boost::recursive_mutex::scoped_lock _l(m_jsObjectMutex);

    while (!m_jsObjectList.empty()) {
        JSObjectRefPtr ptr(m_jsObjectList.back());
        m_jsObjectList.pop_back();
        deferred_release(*ptr->getPtr());
    }
    
    FB::BrowserHost::shutdown();
}

JSObjectRefPtr WebKitBrowserHost::ProtectJSObjectRef(JSObjectRef o)
{
    if (o == NULL) {
        return JSObjectRefPtr();
    }
    boost::recursive_mutex::scoped_lock _l(m_jsObjectMutex);
    JSObjectRefPtr ptr(boost::make_shared<FB::ShareableReference<JSObjectRef> >(new JSObjectRef(o)));
    m_jsObjectList.push_back(ptr);
    JSValueProtect(m_jsContext, *ptr->getPtr());
    
    return ptr;
}
void WebKitBrowserHost::ReleaseJSObjectRef(const JSObjectRefWeakPtr& oref)
{
    boost::recursive_mutex::scoped_lock _l(m_jsObjectMutex);
    JSObjectRefPtr ptr(oref.lock());
    if (ptr) {
        m_jsObjectList.remove(ptr);
        deferred_release(*ptr->getPtr());
        delete ptr->getPtr();
    }
}
void WebKitBrowserHost::deferred_release(JSObjectRef o) const
{
    boost::recursive_mutex::scoped_lock _l(m_deferMutex);
    m_deferredRelease.push_back(o);
    if (isMainThread()) {
        DoDeferredRelease();
    }
}

void WebKitBrowserHost::DoDeferredRelease() const
{
    boost::recursive_mutex::scoped_lock _l(m_deferMutex);
    while (!m_deferredRelease.empty()) {
        JSObjectRef o = m_deferredRelease.back();
        m_deferredRelease.pop_back();
        JSValueUnprotect(m_jsContext, o);
    }
}
