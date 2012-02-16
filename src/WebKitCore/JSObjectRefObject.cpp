/**********************************************************\ 
 Original Author: Richard Bateman (taxilian)
 
 Created:    Oct 16, 2009
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2009 Richard Bateman, Firebreath development team
 \**********************************************************/

#include <typeinfo>
#include "JSObject.h"
#include "JSObjectRefObject.h"

using namespace FB::WebKit;

JSObjectRef JSObjectRefObject::NewObject(const WebKitBrowserHostPtr& host, const FB::JSAPIWeakPtr& api, bool auto_release/* = false*/)
{
    if (!JSObjectRefObjectClass) {
        JSObjectRefObjectClass = JSClassCreate(&JSObjectRefObjectClassDef);
        JSObjectRefObjectUseCount = 1;
    } else {
        JSClassRetain(JSObjectRefObjectClass);
        ++JSObjectRefObjectUseCount;
    }
    JSObjectRef o = JSObjectMake(host->getJSContext(), JSObjectRefObjectClass, NULL);
    JSObjectRefObject *obj = static_cast<JSObjectRefObject *>(JSObjectGetPrivate(o));
    
    if (obj) { // There are certain cases where this may have failed;
               // If obj is null, return null; it's probably during shutdown
        obj->setAPI(api, host);
        obj->m_autoRelease = auto_release;
    }
    return o;
}

JSObjectRefObject::JSObjectRefObject(JSContextRef ctx, JSObjectRef obj)
    : m_valid(true), m_autoRelease(false), m_jsContext(ctx), m_obj(obj),
    m_addEventFunc(boost::make_shared<JSO_addEventListener>(this)),
    m_removeEventFunc(boost::make_shared<JSO_removeEventListener>(this))
{
    m_sharedRef = boost::make_shared<FB::ShareableReference<JSObjectRefObject> >(this);
    
}

JSObjectRefObject::~JSObjectRefObject()
{
    if (m_autoRelease && !m_browser.expired() && !m_api.expired()) {
        // If the JSAPI object is still around and we're set to autorelease, tell the BrowserHost
        // that we're done with it.  Otherwise it's either gone or we don't control its lifecycle
        getHost()->releaseJSAPIPtr(m_api.lock());
    }
    JSClassRelease(JSObjectRefObjectClass);
    if (--JSObjectRefObjectUseCount == 0) {
        JSObjectRefObjectClass = NULL;
    }
}
//
//bool NPJavascriptObject::isNPJavaScriptObject(const NPObject* const npo)
//{
//    return npo->_class == &NPJavascriptObjectClass;
//}

std::string getJSString(JSStringRef strVal)
{
    size_t strSize = JSStringGetMaximumUTF8CStringSize(strVal);
    boost::scoped_array<char> buf(new char[strSize]);
    JSStringGetUTF8CString(strVal, buf.get(), strSize);
    return buf.get();
}

bool JSObjectRefObject::HasProperty(JSStringRef propertyName)
{
    if (!isValid()) return false;
    try {
        WebKitBrowserHostPtr browser(getHost());
        // Handle numeric identifiers
        std::string sName(getJSString(propertyName));
        // We check for events of that name as well in order to allow setting of an event handler in the
        // old javascript style, i.e. plugin.onload = function() .....;
        
        if (sName == "addEventListener" || sName == "removeEventListener") {
            return true;
        } else if (sName != "toString" && getAPI()->HasMethodObject(sName))
            return true;
        else
            return getAPI()->HasProperty(sName);
    } catch (const std::bad_cast&) {
        return false; // invalid object
    } catch (const script_error& e) {
        return false; // Error trying to decide if we support this property :-?
    }    
}
JSValueRef JSObjectRefObject::GetProperty(JSStringRef propertyName, JSValueRef *exception)
{
    if (!isValid()) return JSValueMakeUndefined(m_jsContext);
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        FB::variant res;
        std::string sName(getJSString(propertyName));
        if (sName == "addEventListener") {
            res = m_addEventFunc;
        } else if (sName == "removeEventListener") {
            res = m_removeEventFunc;
        } else if (getAPI()->HasMethodObject(sName)) {
            res = getAPI()->GetMethodObject(sName);
        } else {
            res = getAPI()->GetProperty(sName);
        }
        
        return browser->JSValueRefFromVariant(res);
    } catch (const std::bad_cast&) {
        return JSValueMakeUndefined(m_jsContext);
    } catch (const script_error& e) {
        JSStringRef str = JSStringCreateWithUTF8CString(e.what());
        *exception = JSValueMakeString(m_jsContext, str);
        JSStringRelease(str);
        return JSValueMakeUndefined(m_jsContext);
    }
}
bool JSObjectRefObject::SetProperty(JSStringRef propertyName, JSValueRef value, JSValueRef *exception)
{
    if (!isValid()) return false;
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        FB::variant arg = browser->getVariant(value);
        std::string sName(getJSString(propertyName));
        if (getAPI()->HasMethodObject(sName)) {
            throw FB::script_error("This property cannot be changed");
        } else {
            getAPI()->SetProperty(sName, arg);
        }
        return true;
    } catch (const std::bad_cast&) {
        return false; // invalid object
    } catch(const script_error& e) {
        JSStringRef str = JSStringCreateWithUTF8CString(e.what());
        *exception = JSValueMakeString(m_jsContext, str);
        JSStringRelease(str);
        return false;
    }    
}
bool JSObjectRefObject::DeleteProperty(JSStringRef propertyName, JSValueRef *exception)
{
    if (!isValid()) return false;
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        std::string sName(getJSString(propertyName));
        getAPI()->RemoveProperty(sName);
        return true;
    } catch (const std::bad_cast&) {
        return false; // invalid object
    } catch(const script_error& e) {
        JSStringRef str = JSStringCreateWithUTF8CString(e.what());
        *exception = JSValueMakeString(m_jsContext, str);
        JSStringRelease(str);
        return false;
    }    
}
void JSObjectRefObject::GetPropertyNames(JSPropertyNameAccumulatorRef propertyNames)
{
    if (!isValid()) return;
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        typedef std::vector<std::string> StringArray;
        StringArray memberList;
        getAPI()->getMemberNames(memberList);
        
        for (uint32_t i = 0; i < memberList.size(); i++) {
            JSStringRef curName = JSStringCreateWithUTF8CString(memberList.at(i).c_str());
            JSPropertyNameAccumulatorAddName(propertyNames, curName);
            JSStringRelease(curName);
        }
        JSStringRef addEvt = JSStringCreateWithUTF8CString("addEventListener");
        JSStringRef removeEvt = JSStringCreateWithUTF8CString("removeEventListener");
        JSPropertyNameAccumulatorAddName(propertyNames, addEvt);
        JSPropertyNameAccumulatorAddName(propertyNames, removeEvt);
        JSStringRelease(addEvt);
        JSStringRelease(removeEvt);
    } catch (const std::bad_cast&) {
        // Nothing really to do here
    } catch (const script_error& e) {
        // Nothing really to do here
    }
}
JSValueRef JSObjectRefObject::CallAsFunction(JSObjectRef thisObject, size_t argumentCount,
                                             const JSValueRef arguments[], JSValueRef* exception)
{
    if (!isValid()) return JSValueMakeUndefined(m_jsContext);
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        
        std::vector<FB::variant> vArgs;
        for (unsigned int i = 0; i < argumentCount; i++) {
            vArgs.push_back(browser->getVariant(arguments[i]));
        }
        
        // Default method call
        FB::variant ret = getAPI()->Invoke("", vArgs);
        return browser->JSValueRefFromVariant(ret);
    } catch (const std::bad_cast&) {
         return JSValueMakeUndefined(m_jsContext); // invalid object
    } catch (const script_error& e) {
        JSStringRef str = JSStringCreateWithUTF8CString(e.what());
        *exception = JSValueMakeString(m_jsContext, str);
        JSStringRelease(str);
        return JSValueMakeUndefined(m_jsContext);
    }
}

JSObjectRef JSObjectRefObject::CallAsConstructor(size_t argumentCount, const JSValueRef arguments[],
                                                 JSValueRef* exception)
{
    if (!isValid()) return JSObjectMakeArray(m_jsContext, 0, NULL, exception);
    WebKitBrowserHostPtr browser;
    try {
        browser = getHost();
        
        std::vector<FB::variant> vArgs;
        for (unsigned int i = 0; i < argumentCount; i++) {
            vArgs.push_back(browser->getVariant(arguments[i]));
        }
        // Default method call
        FB::variant ret = getAPI()->Construct(vArgs);
        return JSValueToObject(m_jsContext, browser->JSValueRefFromVariant(ret), exception);
    } catch (const std::bad_cast&) {
        return JSObjectMakeArray(m_jsContext, 0, NULL, exception);
    } catch (const script_error& e) {
        JSStringRef str = JSStringCreateWithUTF8CString(e.what());
        *exception = JSValueMakeString(m_jsContext, str);
        JSStringRelease(str);
        return JSObjectMakeArray(m_jsContext, 0, NULL, exception);
    }
}


FB::variant JSObjectRefObject::JSO_addEventListener::exec( const std::vector<variant>& args )
{
    if (obj->isValid() && args.size() > 1 && args.size() < 4) {
        try {
            std::string evtName = "on" + args[0].convert_cast<std::string>();
            FB::JSObjectPtr method(args[1].convert_cast<FB::JSObjectPtr>());
            obj->getAPI()->registerEventMethod(evtName, method);
            return FB::variant();
        } catch (const std::bad_cast& e) {
            throw FB::invalid_arguments(e.what());
        }
    } else {
        throw FB::invalid_arguments();
    }
}

FB::variant JSObjectRefObject::JSO_removeEventListener::exec( const std::vector<variant>& args )
{
    if (obj->isValid() && args.size() > 1 && args.size() < 4) {
        try {
            std::string evtName = "on" + args[0].convert_cast<std::string>();
            FB::JSObjectPtr method(args[1].convert_cast<FB::JSObjectPtr>());
            obj->getAPI()->unregisterEventMethod(evtName, method);
            return FB::variant();
        } catch (const std::bad_cast& e) {
            throw FB::invalid_arguments(e.what());
        }
    } else {
        throw FB::invalid_arguments();
    }
}

/************************************************************************************\
 These are the static functions given to the browser in the JSClassDefinition struct.
 You might look at these as the "entry points" for the JSObjectRefObject
\************************************************************************************/


void JSObjectRefObject::_Initialize(JSContextRef ctx, JSObjectRef object)
{
    JSObjectRefObject *newObj = new JSObjectRefObject(ctx, object);
    JSObjectSetPrivate(object, newObj);
}

void JSObjectRefObject::_Finalize(JSObjectRef object)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    delete obj;
}

bool JSObjectRefObject::_HasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    return obj->HasProperty(propertyName);
}
JSValueRef JSObjectRefObject::_GetProperty (JSContextRef ctx, JSObjectRef object,
                                            JSStringRef propertyName, JSValueRef *exception)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    return obj->GetProperty(propertyName, exception);
}

bool JSObjectRefObject::_SetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName,
                                     JSValueRef value, JSValueRef *exception)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    return obj->SetProperty(propertyName, value, exception);
}

bool JSObjectRefObject::_DeleteProperty(JSContextRef ctx, JSObjectRef object,
                                        JSStringRef propertyName, JSValueRef *exception)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    return obj->DeleteProperty(propertyName, exception);
}

void JSObjectRefObject::_GetPropertyNames( JSContextRef ctx, JSObjectRef object,
                                          JSPropertyNameAccumulatorRef propertyNames)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(object));
    obj->GetPropertyNames(propertyNames);
}

JSValueRef JSObjectRefObject::_CallAsFunction(JSContextRef ctx, JSObjectRef function,
                                              JSObjectRef thisObject, size_t argumentCount,
                                              const JSValueRef arguments[], JSValueRef* exception)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(function));
    return obj->CallAsFunction(thisObject, argumentCount, arguments, exception);
}

JSObjectRef JSObjectRefObject::_CallAsConstructor(JSContextRef ctx, JSObjectRef constructor,
                                                  size_t argumentCount, const JSValueRef arguments[],
                                                  JSValueRef* exception)
{
    JSObjectRefObject *obj = reinterpreted_cast<JSObjectRefObject *>(JSObjectGetPrivate(constructor));
    return obj->CallAsConstructor(argumentCount, arguments, exception);
}

bool JSObjectRefObject::isJSAPI(JSContextRef jsContext, JSObjectRef o)
{
    if (JSObjectRefObject::JSObjectRefObjectClass && o) {
        return JSValueIsObjectOfClass(jsContext, o, JSObjectRefObjectClass);
    } else {
        return false;
    }
}

JSClassRef JSObjectRefObject::JSObjectRefObjectClass(NULL);
int32_t JSObjectRefObject::JSObjectRefObjectUseCount(0);

// This defines the "entry points"; 
JSClassDefinition JSObjectRefObject::JSObjectRefObjectClassDef = {
    0, // version
    0, // JSClassAttributes
    "JSAPIWrapper", // Class Name
    NULL, // Parent class
    NULL, // static values
    NULL, // static functions
    JSObjectRefObject::_Initialize,
    JSObjectRefObject::_Finalize,
    JSObjectRefObject::_HasProperty,
    JSObjectRefObject::_GetProperty,
    JSObjectRefObject::_SetProperty,
    JSObjectRefObject::_DeleteProperty,
    JSObjectRefObject::_GetPropertyNames,
    JSObjectRefObject::_CallAsFunction,
    JSObjectRefObject::_CallAsConstructor,
    NULL, // hasInstance
    NULL // convertToType
};
