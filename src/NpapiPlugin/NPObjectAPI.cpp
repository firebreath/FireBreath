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
#include "logging.h"
#include <cassert>

using namespace FB::Npapi;

#define RETAIN_COUNT 10
#define REFCOUNT_CUTOFF_MULT 7

NPObjectAPI::NPObjectAPI(NPObject *o, NpapiBrowserHostPtr h)
    : JSObject(h), browser(h), obj(o)
{
    assert(browser);
    if (o != NULL) {
        /*
         * This may look kinda hackish, but some browsers seem to have a bug with reference counting;
         * somehow they get released more times than they should.  This isn't a perfect fix, but it helps.
         * (see ~NPObjectAPI for the rest of the fix)
         */
        for (int i = 0; i < RETAIN_COUNT; i++) {
            browser->RetainObject(obj);
        }
    }
}

NPObjectAPI::~NPObjectAPI(void)
{
    // Force this call to happen on the main thread
    host->CallOnMainThread(boost::bind(&NPObjectAPI::releaseObject, this, obj));
}

void NPObjectAPI::releaseObject(NPObject* obj)
{
    if (obj != NULL) {
        /**
         * Okay, this is far from perfect, but if the reference count
         * is less then RETAIN_COUNT or is higher than
         * RETAIN_COUNT * REFCOUNT_CUTTOFF_MULT then something
         * is really off; to be safe, we'll just avoid doing any releases on 
         * this object; this will absolutely cause a memory leak; however,
         * that's better than a crash.
         **/
        if (obj->referenceCount >= RETAIN_COUNT && obj->referenceCount < (RETAIN_COUNT * REFCOUNT_CUTOFF_MULT)) {
            for (int i = 0; i < RETAIN_COUNT; i++) {
                browser->ReleaseObject(obj);
            }
        } else {
            FBLOG_ERROR("NPObjectAPI", "Invalid reference count found on NPObject!" << obj->referenceCount);
        }
    }
}

void NPObjectAPI::getMemberNames(std::vector<std::string> &nameVector)
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&NPObjectAPI::getMemberNames, this, nameVector));
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;

    browser->Enumerate(obj, &idArray, &count);
    for (uint32_t i = 0; i < count; i++) {
        nameVector.push_back(browser->StringFromIdentifier(idArray[i]));
    }
    browser->MemFree(idArray);
}

size_t NPObjectAPI::getMemberCount()
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&NPObjectAPI::getMemberCount, this));
    }
    NPIdentifier *idArray(NULL);
    uint32_t count;
    browser->Enumerate(obj, &idArray, &count);
    browser->MemFree(idArray);
    return (size_t)count;
}

bool NPObjectAPI::HasMethod(const std::string& methodName)
{
    if (!host->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(const std::string&);
        return host->CallOnMainThread(boost::bind((curtype)&NPObjectAPI::HasMethod, this, methodName));
    }
    return browser->HasMethod(obj, browser->GetStringIdentifier(methodName.c_str()));
}

bool NPObjectAPI::HasProperty(const std::string& propertyName)
{
    if (!host->isMainThread()) {
        typedef bool (NPObjectAPI::*curtype)(const std::string&);
        return host->CallOnMainThread(boost::bind((curtype)&NPObjectAPI::HasProperty, this, propertyName));
    }
    return browser->HasProperty(obj, browser->GetStringIdentifier(propertyName.c_str()));
}

bool NPObjectAPI::HasProperty(int idx)
{
    return browser->HasProperty(obj, browser->GetIntIdentifier(idx));
}

bool NPObjectAPI::HasEvent(const std::string& eventName)
{
    return false;
}


// Methods to manage properties on the API
FB::variant NPObjectAPI::GetProperty(const std::string& propertyName)
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind((FB::GetPropertyType)&JSAPI::GetProperty, this, propertyName));
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
    if (!host->isMainThread()) {
        host->CallOnMainThread(boost::bind((FB::SetPropertyType)&JSAPI::SetProperty, this, propertyName, value));
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
    std::string strIdx(boost::lexical_cast<std::string>(idx));
    return GetProperty(strIdx);
}

void NPObjectAPI::SetProperty(int idx, const FB::variant& value)
{
    std::string strIdx(boost::lexical_cast<std::string>(idx));
    SetProperty(strIdx, value);
}

// Methods to manage methods on the API
FB::variant NPObjectAPI::Invoke(const std::string& methodName, const std::vector<FB::variant>& args)
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind((FB::InvokeType)&NPObjectAPI::Invoke, this, methodName, args));
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
