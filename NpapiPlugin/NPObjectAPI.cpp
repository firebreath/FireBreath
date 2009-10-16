/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NPObjectAPI.h"
#include "NpapiBrowserHost.h"

using namespace FB::Npapi;

NPObjectAPI::NPObjectAPI(NPObject *o, NpapiBrowserHost *h)
    : browser(h), obj(o)
{
    browser->RetainObject(obj);
}

NPObjectAPI::~NPObjectAPI(void)
{
    browser->ReleaseObject(obj);
}

bool NPObjectAPI::HasMethod(std::string methodName)
{
    return browser->HasMethod(obj, browser->GetStringIdentifier(methodName.c_str()));
}

bool NPObjectAPI::HasProperty(std::string propertyName)
{
    return browser->HasProperty(obj, browser->GetStringIdentifier(propertyName.c_str()));
}

bool NPObjectAPI::HasProperty(int idx)
{
    return browser->HasProperty(obj, browser->GetIntIdentifier(idx));
}

bool NPObjectAPI::HasEvent(std::string eventName)
{
    return false;
}


// Methods to manage properties on the API
FB::variant NPObjectAPI::GetProperty(std::string propertyName)
{
    NPVariant retVal;
    if (!browser->GetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &retVal)) {
        throw script_error(propertyName.c_str());
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);
        return ret;
    }
}

void NPObjectAPI::SetProperty(std::string propertyName, const FB::variant value)
{
    NPVariant val;
    browser->getNPVariant(&val, value);
    if (!browser->GetProperty(obj, browser->GetStringIdentifier(propertyName.c_str()), &val)) {
        throw script_error(propertyName.c_str());
    }
}

FB::variant NPObjectAPI::GetProperty(int idx)
{
    NPVariant retVal;
    if (!browser->GetProperty(obj, browser->GetIntIdentifier(idx), &retVal)) {
        throw script_error("Error getting property by index");
    } else {
        FB::variant ret = browser->getVariant(&retVal);
        browser->ReleaseVariantValue(&retVal);
        return ret;
    }
}

void NPObjectAPI::SetProperty(int idx, const FB::variant value)
{
    NPVariant val;
    browser->getNPVariant(&val, value);
    if (!browser->GetProperty(obj, browser->GetIntIdentifier(idx), &val)) {
        throw script_error("Error setting property by index");
    }
}

// Methods to manage methods on the API
FB::variant NPObjectAPI::Invoke(std::string methodName, std::vector<FB::variant>& args)
{
    NPVariant retVal;

    // Convert the arguments to NPVariants
    NPVariant *npargs = new NPVariant[args.size()];
    for (unsigned int i = 0; i < args.size(); i++) {
        browser->getNPVariant(&npargs[i], args[i]);
    }

    bool res = false;
    // Invoke the method ("" means invoke default method)
    if (methodName.size() > 0) {
        res = browser->Invoke(obj, browser->GetStringIdentifier(methodName.c_str()), npargs, args.size(), &retVal);
    } else {
        res = browser->InvokeDefault(obj, npargs, args.size(), &retVal);
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