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

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>
#include "NPObjectAPI.h"
#include "NpapiBrowserHost.h"
#include "NPJavascriptObject.h"
#include "logging.h"
#include <cassert>

using namespace FB::Npapi;

NPObjectAPI::NPObjectAPI(NPObject *o, const NpapiBrowserHostPtr& h)
    : JSObject(h), m_browser(h), obj(o), is_JSAPI(false)
{
    assert(!m_browser.expired());
    if (o != NULL) {
        getHost()->RetainObject(obj);
    }
    FB::JSAPIPtr ptr(getJSAPI());
    if (ptr) {
        // This is a JSAPI object wrapped in an IDispatch object; we'll call it
        // directly(ish)
        is_JSAPI = true;
        inner = ptr;
    }
}

NPObjectAPI::~NPObjectAPI(void)
{
    // Schedule the NPObject for release on the main thread
    if (!m_browser.expired())
        getHost()->deferred_release(obj);
    obj = NULL;
}

void NPObjectAPI::getMemberNames(std::vector<std::string> &nameVector) const
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef void (FB::JSAPI::*getMemberNamesType)(std::vector<std::string> *nameVector) const;
        browser->CallOnMainThread(boost::bind((getMemberNamesType)&FB::JSAPI::getMemberNames, this, &nameVector));
        return;
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            tmp->getMemberNames(nameVector);
        return;
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;

    browser->Enumerate(obj, &idArray, &count);
    for (uint32_t i = 0; i < count; i++) {
        nameVector.push_back(browser->StringFromIdentifier(idArray[i]));
    }
    browser->MemFree(idArray);
}

size_t NPObjectAPI::getMemberCount() const
{
    if (m_browser.expired())
        return 0;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(boost::bind(&NPObjectAPI::getMemberCount, this));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->getMemberCount();
        else 
            return 0;
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;
    browser->Enumerate(obj, &idArray, &count);
    browser->MemFree(idArray);
    return (size_t)count;
}

bool NPObjectAPI::HasMethod(const std::string& methodName) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(const std::string&) const;
        return browser->CallOnMainThread(boost::bind((curtype)&NPObjectAPI::HasMethod, this, methodName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasMethod(methodName);
        else 
            return false;
    }
    return browser->HasMethod(obj, browser->GetStringIdentifier(methodName.c_str()));
}

bool NPObjectAPI::HasProperty(const std::string& propertyName) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(const std::string&) const;
        return browser->CallOnMainThread(boost::bind((curtype)&NPObjectAPI::HasProperty, this, propertyName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasProperty(propertyName);
        else 
            return false;
    }
    return browser->HasProperty(obj, browser->GetStringIdentifier(propertyName.c_str()));
}

bool NPObjectAPI::HasProperty(int idx) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasProperty(idx);
        else 
            return false;
    }
    return browser->HasProperty(obj, browser->GetIntIdentifier(idx));
}

bool NPObjectAPI::HasEvent(const std::string& eventName) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasEvent(eventName);
        else 
            return false;
    }
    return false;
}


// Methods to manage properties on the API
FB::variant NPObjectAPI::GetProperty(const std::string& propertyName)
{
    if (m_browser.expired())
        return FB::FBVoid();

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(boost::bind((FB::GetPropertyType)&JSAPI::GetProperty, this, propertyName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->GetProperty(propertyName);
        else 
            return false;
    }
    NPVariant retVal;
    if (!browser->GetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &retVal)) {
        throw script_error(propertyName.c_str());
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);
        return ret;
    }
}

void NPObjectAPI::SetProperty(const std::string& propertyName, const FB::variant& value)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        browser->CallOnMainThread(boost::bind((FB::SetPropertyType)&JSAPI::SetProperty, this, propertyName, value));
        return;
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            SetProperty(propertyName, value);
        return;
    }
    NPVariant val;
    browser->getNPVariant(&val, value);
    if (!browser->SetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &val)) {
        throw script_error(propertyName.c_str());
    }
}

FB::variant NPObjectAPI::GetProperty(int idx)
{
    if (m_browser.expired())
        return FB::FBVoid();

    NpapiBrowserHostPtr browser(getHost());
    std::string strIdx(boost::lexical_cast<std::string>(idx));
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->GetProperty(idx);
    }
    return GetProperty(strIdx);
}

void NPObjectAPI::SetProperty(int idx, const FB::variant& value)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    std::string strIdx(boost::lexical_cast<std::string>(idx));
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            SetProperty(idx, value);
    }
    SetProperty(strIdx, value);
}

// Methods to manage methods on the API
FB::variant NPObjectAPI::Invoke(const std::string& methodName, const std::vector<FB::variant>& args)
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(boost::bind((FB::InvokeType)&NPObjectAPI::Invoke, this, methodName, args));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->Invoke(methodName, args);
        else 
            return false;
    }
    NPVariant retVal;

    // Convert the arguments to NPVariants
    boost::scoped_array<NPVariant> npargs(new NPVariant[args.size()]);
    for (unsigned int i = 0; i < args.size(); i++) {
        browser->getNPVariant(&npargs[i], args[i]);
    }

    bool res = false;
    // Invoke the method ("" means invoke default method)
    if (methodName.size() > 0) {
        res = browser->Invoke(obj, browser->GetStringIdentifier(methodName.c_str()), npargs.get(), args.size(), &retVal);
    } else {
        res = browser->InvokeDefault(obj, npargs.get(), args.size(), &retVal);
    }

    // Free the NPVariants that we earlier allocated
    for (unsigned int i = 0; i < args.size(); i++) {
        browser->ReleaseVariantValue(&npargs[i]);
    }

    if (!res) { // If the method call failed, throw an exception
        throw script_error(methodName.c_str());
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);  // Always release the return value!
        return ret;
    }
}

//FB::JSObjectPtr NPObjectAPI::Construct( const std::string& memberName, const FB::VariantList& args )
//{
//    // NOTE: This doesn't work.  Sorry :-/
//    if (!host->isMainThread()) {
//        typedef FB::JSObjectPtr (FB::JSObject::*curtype)(const std::string& memberName, const std::vector<variant>& args);
//        return host->CallOnMainThread(boost::bind((curtype)&JSObject::Construct, this, memberName, args));
//    }
//    NPVariant retVal;
//
//    if (!memberName.empty()) {
//        if (memberName != "constructor")
//            return this->GetProperty(memberName).convert_cast<FB::JSObjectPtr>()->Construct("constructor", args);
//        else
//            return this->GetProperty("constructor").convert_cast<FB::JSObjectPtr>()->Construct("", args);
//    }
//
//    // Convert the arguments to NPVariants
//    boost::scoped_array<NPVariant> npargs(new NPVariant[args.size()]);
//    for (unsigned int i = 0; i < args.size(); i++) {
//        browser->getNPVariant(&npargs[i], args[i]);
//    }
//
//    bool res = false;
//    // Construct the new object
//    res = browser->Construct(obj, npargs.get(), args.size(), &retVal);
//
//    // Free the NPVariants that we earlier allocated
//    for (unsigned int i = 0; i < args.size(); i++) {
//        browser->ReleaseVariantValue(&npargs[i]);
//    }
//
//    if (!res) { // If the method call failed, throw an exception
//        throw script_error("Could not construct object");
//    } else {
//        FB::JSObjectPtr ret = browser->getVariant(&retVal).convert_cast<FB::JSObjectPtr>();
//        browser->ReleaseVariantValue(&retVal);  // Always release the return value!
//        return ret;
//    }
//}

FB::JSAPIPtr NPObjectAPI::getJSAPI() const
{
    if (!obj) {
        return JSAPIPtr();
    }
    
    if (!NPJavascriptObject::isNPJavaScriptObject(obj)) {
        return JSAPIPtr();
    }
    
    return static_cast<NPJavascriptObject*>(obj)->getAPI();
}

