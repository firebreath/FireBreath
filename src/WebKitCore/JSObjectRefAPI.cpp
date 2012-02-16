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

#include <boost/scoped_array.hpp>
#include "JSObjectRefObject.h"
#include "JSObjectRefAPI.h"

using namespace FB::WebKit;
using namespace FB;

JSObjectRefAPI::JSObjectRefAPI(JSObjectRef o, const WebKitBrowserHostPtr& h)
    : JSObject(h), m_browser(h), is_JSAPI(false)
{
    assert(!m_browser.expired());
    m_jsContext = h->getJSContext();
    obj = h->ProtectJSObjectRef(o);
}

JSObjectRefAPI::~JSObjectRefAPI(void)
{
    WebKitBrowserHostPtr host(m_browser.lock());
    if (host) {
        host->ReleaseJSObjectRef(obj);
    }
}

void JSObjectRefAPI::getMemberNames(std::vector<std::string> &nameVector) const
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return;
    
    JSPropertyNameArrayRef nameArray = JSObjectCopyPropertyNames(m_jsContext, *o->getPtr());
    
    size_t count = JSPropertyNameArrayGetCount(nameArray);
    for (size_t i = 0; i < count; ++i) {
        JSStringRef curStr = JSPropertyNameArrayGetNameAtIndex(nameArray, i);
        size_t strSize = JSStringGetMaximumUTF8CStringSize(curStr);
        boost::scoped_array<char> buf(new char[strSize]);
        JSStringGetUTF8CString(curStr, buf.get(), strSize);
        nameVector.push_back(buf.get());
    }
    JSPropertyNameArrayRelease(nameArray);
}

JSAPIPtr JSObjectRefAPI::getJSAPI() const
{
    JSObjectRefPtr o(obj.lock());
    
    if (JSObjectRefObject::isJSAPI(m_jsContext, *o->getPtr())) {
        JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(*o->getPtr()));
        return obj->getAPI();
    }
    
    return JSAPIPtr();
}

size_t JSObjectRefAPI::getMemberCount() const
{
    JSObjectRefPtr o(obj.lock());
    if (!o)
        return 0;
    JSPropertyNameArrayRef nameArray = JSObjectCopyPropertyNames(m_jsContext, *o->getPtr());
    size_t count = JSPropertyNameArrayGetCount(nameArray);
    JSPropertyNameArrayRelease(nameArray);
    return count;
}

bool JSObjectRefAPI::HasMethod(const std::string& methodName) const
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return false;

    if (methodName.empty()) {
        return JSObjectIsFunction(m_jsContext, *o->getPtr());
    } else {
        JSStringRef str = JSStringCreateWithUTF8CString(methodName.c_str());
        if (!JSObjectHasProperty(m_jsContext, *o->getPtr(), str)) {
            JSStringRelease(str);
            return false;
        }
        JSValueRef except(NULL);
        JSValueRef inner = JSObjectGetProperty(m_jsContext, *o->getPtr(), str, &except);
        JSStringRelease(str);
        if (except != NULL) {
            // Hmm.  Do something here. Not sure what, actually.
        } else if (JSValueIsObject(m_jsContext, inner)) {
            return JSObjectIsFunction(m_jsContext, JSValueToObject(m_jsContext, inner, &except));
        }
    }
    return false;
}
bool JSObjectRefAPI::HasProperty(const std::string& propertyName) const
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return false;
    
    JSStringRef str = JSStringCreateWithUTF8CString(propertyName.c_str());
    bool exists = JSObjectHasProperty(m_jsContext, *o->getPtr(), str);
    JSStringRelease(str);
    return exists;
}
bool JSObjectRefAPI::HasProperty(int idx) const
{
    return HasProperty(boost::lexical_cast<std::string>(idx));
}


variant JSObjectRefAPI::GetProperty(const std::string& propertyName)
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return FBVoid();
    
    JSValueRef except(NULL);
    JSStringRef str = JSStringCreateWithUTF8CString(propertyName.c_str());
    JSValueRef res = JSObjectGetProperty(m_jsContext, *o->getPtr(), str, &except);
    JSStringRelease(str);
    
    if (except != NULL) {
        throw FB::script_error("Could not get property " + propertyName);
        // TODO: Handle the exception; how?
    } else {
        return getHost()->getVariant(res);
    }
}
void JSObjectRefAPI::SetProperty(const std::string& propertyName, const variant& value)
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return;
    JSObjectRef obref(*o->getPtr());
    JSValueProtect(m_jsContext, obref);
    
    JSValueRef except(NULL);
    JSStringRef str = JSStringCreateWithUTF8CString(propertyName.c_str());
    
    JSValueRef newVal = getHost()->JSValueRefFromVariant(value);
    JSObjectSetProperty(m_jsContext, obref, str, newVal, kJSPropertyAttributeNone, &except);

    JSStringRelease(str);
    JSValueUnprotect(m_jsContext, obref);
    
    if (except != NULL) {
        throw FB::script_error("Couldn't set property");
        // TODO: Handle the exception; how?
    }
}
void JSObjectRefAPI::RemoveProperty(const std::string& propertyName)
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return;
    
    JSValueRef except(NULL);
    JSStringRef str = JSStringCreateWithUTF8CString(propertyName.c_str());
    bool res = JSObjectDeleteProperty(m_jsContext, *o->getPtr(), str, &except);
    JSStringRelease(str);
    
    if (!res || except != NULL) {
        throw FB::script_error("Could not remove property " + propertyName);
    }
}
variant JSObjectRefAPI::GetProperty(int idx)
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return FBVoid();
    
    JSValueRef except(NULL);
    JSValueRef res = JSObjectGetPropertyAtIndex(m_jsContext, *o->getPtr(), idx, &except);
    
    if (except != NULL) {
        throw FB::script_error("Could not get property at index");
        // TODO: Handle the exception; how?
    } else {
        return getHost()->getVariant(res);
    }
}
void JSObjectRefAPI::SetProperty(int idx, const variant& value)
{
    JSObjectRefPtr o(obj.lock());
    if (!o || m_browser.expired())
        return;
    
    JSValueRef except(NULL);
    JSValueRef newVal = getHost()->JSValueRefFromVariant(value);
    JSObjectSetPropertyAtIndex(m_jsContext, *o->getPtr(), idx, newVal, &except);
    
    if (except != NULL) {
        throw FB::script_error("Couldn't set property");
        // TODO: Handle the exception; how?
    }
}
void JSObjectRefAPI::RemoveProperty(int idx)
{
    RemoveProperty(boost::lexical_cast<std::string>(idx));
}

variant JSObjectRefAPI::Invoke(const std::string& methodName, const std::vector<variant>& args)
{
    JSObjectRefPtr o(obj.lock());
    WebKitBrowserHostPtr browser(m_browser.lock());
    if (!o || !browser)
        throw FB::object_invalidated();
    
    size_t argCount(args.size());
    boost::scoped_array<JSValueRef> jsArgs(new JSValueRef[argCount]);
    
    for (size_t i = 0; i < argCount; ++i) {
        jsArgs[i] = browser->JSValueRefFromVariant(args[i]);
    }
    JSValueRef except(NULL);
    JSValueRef res = JSObjectCallAsFunction(m_jsContext, *o->getPtr(), *o->getPtr(), argCount, jsArgs.get(), &except); 
    if (except != NULL) {
        FB::variant var(browser->getVariant(except));
        try {
            throw FB::script_error(var.convert_cast<std::string>());
        }
        catch (...) {
            throw FB::script_error("Couldn't Invoke");
        }
    }
    return browser->getVariant(res);
}

variant JSObjectRefAPI::Construct(const std::vector<variant>& args)
{
    JSObjectRefPtr o(obj.lock());
    WebKitBrowserHostPtr browser(m_browser.lock());
    if (!o || !browser)
        throw FB::object_invalidated();
    
    size_t argCount(args.size());
    boost::scoped_array<JSValueRef> jsArgs(new JSValueRef[argCount]);
    
    for (size_t i = 0; i < argCount; ++i) {
        jsArgs[i] = browser->JSValueRefFromVariant(args[i]);
    }
    JSValueRef except(NULL);
    JSValueRef res = JSObjectCallAsConstructor(m_jsContext, *o->getPtr(), argCount, jsArgs.get(), &except);
    
    if (except != NULL) {
        throw FB::script_error("Couldn't construct object");
    }
    return browser->getVariant(res);
}
