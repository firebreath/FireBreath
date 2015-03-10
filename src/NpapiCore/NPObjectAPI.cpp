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
#include "NPObjectAPI.h"
#include "NPJavascriptObject.h"
#include "NpapiBrowserHost.h"
#include "logging.h"
#include <cassert>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB::Npapi;
using FB::JSObjectPtr;
using std::static_pointer_cast;

NPObjectAPI::NPObjectAPI(NPObject *o, const NpapiBrowserHostPtr& h)
    : JSObject(h), m_browser(h), obj(o)
{
    assert(!m_browser.expired());
    if (o != NULL) {
        getHost()->RetainObject(obj);
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
        browser->CallOnMainThread(std::bind((getMemberNamesType)&FB::JSAPI::getMemberNames, this, &nameVector));
        return;
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;

    browser->Enumerate(obj, &idArray, &count);
    for (uint32_t i = 0; i < count; i++) {
        nameVector.emplace_back(browser->StringFromIdentifier(idArray[i]));
    }
    browser->MemFree(idArray);
}

size_t NPObjectAPI::getMemberCount() const
{
    if (m_browser.expired())
        return 0;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(std::bind(&NPObjectAPI::getMemberCount, this));
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;
    browser->Enumerate(obj, &idArray, &count);
    browser->MemFree(idArray);
    return (size_t)count;
}

bool NPObjectAPI::HasMethod(std::string methodName) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(std::string) const;
        return browser->CallOnMainThread(std::bind((curtype)&NPObjectAPI::HasMethod, this, methodName));
    }
    return browser->HasMethod(obj, browser->GetStringIdentifier(methodName.c_str()));
}

bool NPObjectAPI::HasProperty(std::string propertyName) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(std::string) const;
        return browser->CallOnMainThread(std::bind((curtype)&NPObjectAPI::HasProperty, this, propertyName));
    }
    return browser->HasProperty(obj, browser->GetStringIdentifier(propertyName.c_str()));
}

bool NPObjectAPI::HasProperty(int idx) const
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    return browser->HasProperty(obj, browser->GetIntIdentifier(idx));
}

// Methods to manage properties on the API
FB::variantPromise NPObjectAPI::GetProperty(std::string propertyName) {
    return GetPropertySync(propertyName);
}

FB::variant NPObjectAPI::GetPropertySync(std::string propertyName)
{
    if (m_browser.expired())
        return FB::FBVoid();

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        using curType = variant(JSAPI::*)(std::string);
        return browser->CallOnMainThread(std::bind((curType)&NPObjectAPI::GetPropertySync, this, propertyName));
    }
    NPVariant retVal;
    if (!browser->GetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &retVal)) {
        browser->ReleaseVariantValue(&retVal);
        throw script_error(propertyName.c_str());
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);
        return ret;
    }
}

void NPObjectAPI::SetProperty(std::string propertyName, const FB::variant& value)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        browser->CallOnMainThread(std::bind((FB::SetPropertyType)&JSAPI::SetProperty, this, propertyName, value));
        return;
    }
    NPVariant val;
    browser->getNPVariant(&val, value);
    bool res = browser->SetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &val);
    browser->ReleaseVariantValue(&val);
    if (!res) {
        throw script_error(propertyName.c_str());
    }
}

void NPObjectAPI::RemoveProperty(std::string propertyName)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(std::bind((FB::RemovePropertyType)&JSAPI::RemoveProperty, this, propertyName));
    }
    if (!browser->RemoveProperty(obj, browser->GetStringIdentifier(propertyName.c_str()))) {
        throw script_error(propertyName.c_str());
    }
}

FB::variantPromise NPObjectAPI::GetProperty(int idx) {
    return GetPropertySync(idx);
}
FB::variant NPObjectAPI::GetPropertySync(int idx)
{
    if (m_browser.expired())
        return FB::FBVoid();

    NpapiBrowserHostPtr browser(getHost());
    std::string strIdx(std::to_string(idx));
    return GetPropertySync(strIdx);
}

void NPObjectAPI::SetProperty(int idx, const FB::variant& value)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    std::string strIdx(std::to_string(idx));
    SetProperty(strIdx, value);
}

void NPObjectAPI::RemoveProperty(int idx)
{
    if (m_browser.expired())
        return;

    NpapiBrowserHostPtr browser(getHost());
    std::string strIdx(std::to_string(idx));
    return RemoveProperty(strIdx);
}

// Methods to manage methods on the API
FB::variantPromise NPObjectAPI::Invoke(std::string methodName, const std::vector<FB::variant>& args) {
    return InvokeSync(methodName, args);
}
FB::variant NPObjectAPI::InvokeSync(std::string methodName, const std::vector<FB::variant>& args)
{
    if (m_browser.expired())
        return false;

    NpapiBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        auto cb = (FB::variant(NPObjectAPI::*)(std::string, const FB::VariantList&))&NPObjectAPI::InvokeSync;
        return browser->CallOnMainThread(std::bind(cb, this, methodName, args));
    }
    NPVariant retVal;

    // Convert the arguments to NPVariants
    std::unique_ptr<NPVariant[]> npargs(new NPVariant[args.size()]);
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
        browser->ReleaseVariantValue(&retVal);  // Always release the return value!
        throw script_error(methodName.c_str());
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);  // Always release the return value!
        return ret;
    }
}

void NPObjectAPI::callMultipleFunctions( std::string name, const FB::VariantList& args, const std::vector<JSObjectPtr>& direct, const std::vector<JSObjectPtr>& ifaces )
{
    if (!isValid())
        throw FB::script_error("Error calling handlers");

    auto browser(getHost());
    if (!browser->isMainThread()) {
        return browser->ScheduleOnMainThread(shared_from_this(), std::bind(&NPObjectAPI::callMultipleFunctions, this, name, args, direct, ifaces));
    }
    NPVariant retVal;

    // We make these calls through a helper javascript function that is injected into
    // the page on startup.  The reason we do this is to prevent some weird reentrance bugs
    // particularly in FF4.
    
    auto helper = browser->getJSHelper();
    NPIdentifier idFn = browser->GetStringIdentifier("asyncCall");

    // Allocate the arguments
    std::unique_ptr<NPVariant[]> npargs(new NPVariant[4]);
    browser->getNPVariant(&npargs[0], 0);
    browser->getNPVariant(&npargs[2], args);
    browser->getNPVariant(&npargs[3], name);
    
    bool res = false;
    for (auto obj : direct) {
        NPObjectAPIPtr ptr(std::static_pointer_cast<NPObjectAPI>(obj));
        browser->getNPVariant(&npargs[1], ptr);
        res = browser->Invoke(helper, idFn, npargs.get(), 3, &retVal);
        browser->ReleaseVariantValue(&retVal);
        browser->ReleaseVariantValue(&npargs[1]);
    }
    
    for (auto obj : ifaces) {
        NPObjectAPIPtr ptr(std::static_pointer_cast<NPObjectAPI>(obj));
        browser->getNPVariant(&npargs[1], ptr);
        res = browser->Invoke(helper, idFn, npargs.get(), 4, &retVal);
        browser->ReleaseVariantValue(&retVal);
        browser->ReleaseVariantValue(&npargs[1]);
    }
    browser->ReleaseVariantValue(&npargs[2]);
    browser->ReleaseVariantValue(&npargs[3]);
}
