/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 4, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "win_targetver.h"
#include <dispex.h>
#include "utf8_tools.h"
#include "JSAPI_IDispatchEx.h"
#include "axutil.h"

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "IDispatchAPI.h"

using namespace FB::ActiveX;

std::shared_ptr<FB::ActiveX::IDispatchAPI> IDispatchAPI::create(IDispatch * obj, const ActiveXBrowserHostPtr& host)
{
    return std::shared_ptr<IDispatchAPI>(new IDispatchAPI(obj, host));
}

FB::ActiveX::IDispatchAPI::IDispatchAPI(IDispatch * obj, const ActiveXBrowserHostPtr& host) :
    FB::JSObject(host), m_browser(host), m_obj(host->getIDispatchRef(obj)), is_JSAPI(false)
{
    FB::JSAPIPtr ptr(getJSAPI());
    
    if (ptr) {
        // This is a JSAPI object wrapped in an IDispatch object; we'll call it
        // directly(ish)
        is_JSAPI = true;
        inner = ptr;
    }
}
IDispatchAPI::~IDispatchAPI(void)
{
    if (!m_browser.expired())
        getHost()->deferred_release(m_obj);
    m_obj.reset();
}

void IDispatchAPI::getMemberNames(std::vector<std::string> &nameVector) const
{
    if (m_browser.expired() || m_obj.expired())
        return;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef void (FB::JSAPI::*getMemberNamesType)(std::vector<std::string> *nameVector) const;
        browser->CallOnMainThread(std::bind((getMemberNamesType)&FB::JSAPI::getMemberNames, this, &nameVector));
        return;
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            tmp->getMemberNames(nameVector);
        return;
    }

    try {
        CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
        if (!dispatchEx) {
            throw FB::script_error("Cannot enumerate members; IDispatchEx not supported");
        }

        DISPID dispid = DISPID_STARTENUM;
        while (dispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid) != S_FALSE) {
            if (dispid < 0) {
                continue;
            }
            CComBSTR memberName;
            if (SUCCEEDED(dispatchEx->GetMemberName(dispid, &memberName))) {
                std::wstring name(memberName);
                nameVector.emplace_back(FB::wstring_to_utf8(name));
            }
        }
    } catch(...) {}
}

size_t IDispatchAPI::getMemberCount() const
{
    if (m_browser.expired() || m_obj.expired())
        return 0;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(std::bind(&IDispatchAPI::getMemberCount, this));
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (!tmp) {
            // TODO: check if this should be -1
            return 0;
        }
        return tmp->getMemberCount();
    }

    size_t count = 0;
    try {
        CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
        if (!dispatchEx) {
            return -1;
        }

        DISPID dispid = DISPID_STARTENUM;    
        while (dispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid) != S_FALSE) {
            if (dispid >= 0) {
                ++count;
            }
        }
    } catch (...) {}

    return count;
}

DISPID IDispatchAPI::getIDForName(const std::wstring& name) const
{
    if (m_browser.expired() || m_obj.expired())
        return DISPID_UNKNOWN;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->CallOnMainThread(std::bind(&IDispatchAPI::getIDForName, this, name));
    }

    if (name.empty()) {
        return DISPID_VALUE;
    }

    HRESULT hr = E_NOTIMPL;
    DISPID dispId = DISPID_UNKNOWN;
    try {
        CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
        if (dispatchEx) {
            hr = dispatchEx->GetDispID(CComBSTR(name.c_str()),
                fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);
        } else {
            const wchar_t* p = name.c_str();
            hr = getIDispatch()->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&p), 1, LOCALE_SYSTEM_DEFAULT, &dispId);
        }

        if (FAILED(hr)) {
            if (hr == E_NOTIMPL) {
                return AxIdMap.getIdForValue(name); // Makes events possible
            }
            return DISPID_UNKNOWN;
        }
    } catch (...) {}
    return dispId;
}

bool IDispatchAPI::HasMethod(const std::wstring& methodName) const
{
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(methodName) != -1;
}

bool IDispatchAPI::HasMethod(std::string methodName) const
{
    if (m_browser.expired() || m_obj.expired())
        return false;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (IDispatchAPI::*curtype)(std::string) const;
        return browser->CallOnMainThread(std::bind((curtype)&IDispatchAPI::HasMethod, this, methodName));
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (!tmp) {
            return false;
        }
        return tmp->HasMethod(methodName);
    }

    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(FB::utf8_to_wstring(methodName)) != -1 && !HasProperty(methodName);
}

bool IDispatchAPI::HasProperty(const std::wstring& propertyName) const
{
    return HasProperty(FB::wstring_to_utf8(propertyName));
}

bool IDispatchAPI::HasProperty(std::string propertyName) const
{
    if (m_browser.expired() || m_obj.expired())
        return false;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        typedef bool (IDispatchAPI::*HasPropertyType)(std::string) const;
        return browser->CallOnMainThread(std::bind((HasPropertyType)&IDispatchAPI::HasProperty, this, propertyName));
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (!tmp) {
            return false;
        }
        return tmp->HasProperty(propertyName);
    }

    DISPID dispId = getIDForName(FB::utf8_to_wstring(propertyName));
    if (dispId == DISPID_UNKNOWN && propertyName != "toString") {
        return false;
    }

    DISPPARAMS params;
    params.cArgs = 0;
    params.cNamedArgs = 0;

    // the only way to find out if the property actually exists or not is to try to get it
    HRESULT hr;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    try {
        CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
        if (dispatchEx) {
            hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, 
                DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, nullptr);
        } else {
            hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
                DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, nullptr);
        }
        return SUCCEEDED(hr);
    } catch (...) { return false; }
}

bool IDispatchAPI::HasProperty(int idx) const
{
    FB::variant name(idx);
    return getIDForName(name.convert_cast<std::wstring>()) != -1;
}


FB::variantPromise IDispatchAPI::GetProperty(std::string propertyName) {
    return GetPropertySync(propertyName);
}

// Methods to manage properties on the API
FB::variant FB::ActiveX::IDispatchAPI::GetPropertySync(std::string propertyName) {
    if (m_browser.expired() || m_obj.expired())
        return FB::FBVoid();

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        using curType = variant(JSAPI::*)(std::string);
        return browser->CallOnMainThread(std::bind((curType)&IDispatchAPI::GetPropertySync, this, propertyName));
    }

    DISPID dispId = getIDForName(FB::utf8_to_wstring(propertyName));
    if (dispId == DISPID_UNKNOWN && propertyName != "toString") {
        throw FB::script_error("Could not get property");
    }

    // TODO: how can toString == DISPID_UNKNOWN work?

    DISPPARAMS params;
    params.cArgs = 0;
    params.cNamedArgs = 0;

    HRESULT hr;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    try {
        CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
        if (dispatchEx) {
            hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params,
                &result, &exceptionInfo, nullptr);
        } else {
            hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
                DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, nullptr);
        }

        if (FAILED(hr)) {
            throw FB::script_error("Could not get property");
        }

        return browser->getVariant(&result);
    } catch (...) {
        throw FB::script_error("Could not get property");
    }
}

void IDispatchAPI::SetProperty(std::string propertyName, const FB::variant& value)
{
    if (m_browser.expired() || m_obj.expired())
        return;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        browser->CallOnMainThread(std::bind((FB::SetPropertyType)&IDispatchAPI::SetProperty, this, propertyName, value));
        return;
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            tmp->SetProperty(propertyName, value);
        return;
    }

    DISPID dispId = getIDForName(FB::utf8_to_wstring(propertyName));
    if (dispId == DISPID_UNKNOWN) {
        throw FB::script_error("Could not set property");
    }

    CComVariant arg[1];
    VARIANTARG rawArg[1];
    DISPID namedArg[1];
    DISPPARAMS params;
    params.cArgs = 1;
    params.cNamedArgs = 1;
    params.rgdispidNamedArgs = namedArg;
    params.rgvarg = rawArg;

    browser->getComVariant(&arg[0], value);
    rawArg[0] = arg[0];
    namedArg[0] = DISPID_PROPERTYPUT;

    HRESULT hr;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
    if (dispatchEx) {
        hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &params,
            &result, &exceptionInfo, nullptr);
        if (hr == DISP_E_MEMBERNOTFOUND) {
            hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &params,
                nullptr, &exceptionInfo, nullptr);
        }
    } else {
        hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYPUT, &params, &result, &exceptionInfo, nullptr);
    }

    if (FAILED(hr)) {
        throw FB::script_error("Could not set property");
    }
}

void IDispatchAPI::RemoveProperty(std::string propertyName)
{
    if (m_browser.expired())
        return;

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        browser->CallOnMainThread(std::bind((FB::RemovePropertyType)&IDispatchAPI::RemoveProperty, this, propertyName));
        return;
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            RemoveProperty(propertyName);
        return;
    }

    DISPID dispId = getIDForName(FB::utf8_to_wstring(propertyName));
    if (dispId == DISPID_UNKNOWN) {
        return;
    }

    HRESULT hr;
    CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
    if (dispatchEx) {
        hr = dispatchEx->DeleteMemberByDispID(dispId);
    } else {
        // todo: IDispatch does not support removing properties. Do SetProperty(propertyName, undefined); instead?
        throw FB::script_error("Could not remove property");
    }

    if (FAILED(hr)) {
        return;
    } else {
        // todo: we should probably remove the name from the afxmap, but this is a bit hairy as described at
        //        http://msdn.microsoft.com/en-us/library/sky96ah7(v=vs.94).aspx . So we'll ignore it for now.
    }
}


FB::variantPromise IDispatchAPI::GetProperty(int idx) {
    return GetPropertySync(idx);
}
FB::variant IDispatchAPI::GetPropertySync(int idx)
{
    FB::variant sIdx(idx);
    return GetPropertySync(sIdx.convert_cast<std::string>());
}

void IDispatchAPI::RemoveProperty(int idx)
{
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            RemoveProperty(idx);
    }

    FB::variant sIdx(idx);
    RemoveProperty(sIdx.convert_cast<std::string>());
}

void IDispatchAPI::SetProperty(int idx, const FB::variant& value)
{
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            tmp->SetProperty(idx, value);
    }

    FB::variant sIdx(idx);
    SetProperty(sIdx.convert_cast<std::string>(), value);
}


// Methods to manage methods on the API
FB::variantPromise IDispatchAPI::Invoke(std::string methodName, const std::vector<FB::variant>& args) {
    // TODO: Make this asynchronous
    return InvokeSync(methodName, args);
}

FB::variant IDispatchAPI::InvokeSync(std::string methodName, const std::vector<FB::variant>& args)
{
    if (m_browser.expired() || m_obj.expired())
        return FB::FBVoid();

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        auto cb = (FB::variant(IDispatchAPI::*)(std::string, const FB::VariantList&))&IDispatchAPI::InvokeSync;
        return browser->CallOnMainThread(std::bind(cb, this, methodName, args));
    }

    DISPID dispId = getIDForName(FB::utf8_to_wstring(methodName));
    if (dispId == DISPID_UNKNOWN) {
         throw FB::script_error("Method invoke failed");
    }

    size_t argCount(args.size());
    std::unique_ptr<CComVariant[]> comArgs(new CComVariant[argCount + 1]);
    std::unique_ptr<VARIANTARG[]> rawComArgs(new VARIANTARG[argCount + 1]);
    DISPPARAMS params;
    DISPID tid = DISPID_THIS;
    params.cArgs = args.size() + 1;
    params.cNamedArgs = 1;
    params.rgvarg = rawComArgs.get();
    params.rgdispidNamedArgs = &tid; // Needed for IE9
    
    for (size_t i = 0; i < argCount; i++) {
        browser->getComVariant(&comArgs[argCount - i], args[i]);
        // We copy w/out adding a ref so that comArgs will still clean up the values when it goes away
        rawComArgs[argCount - i] = comArgs[argCount - i];
    }
    comArgs[0] = getIDispatch(); // Needed for IE 9
    rawComArgs[0] = comArgs[0];

    CComVariant result;
    CComExcepInfo exceptionInfo;
    CComQIPtr<IDispatchEx> dispatchEx(getIDispatch());
    HRESULT hr;
    if (!dispatchEx) {
        hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
    } else {
        hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT,
            DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
    }
    if (FAILED(hr)) {
        throw FB::script_error("Method invoke failed");
    }
    
    return browser->getVariant(&result);
}

void IDispatchAPI::callMultipleFunctions( std::string name, const FB::VariantList& args, 
                                          const std::vector<FB::JSObjectPtr>& direct, const std::vector<FB::JSObjectPtr>& ifaces )
{
    if (!isValid())
        throw FB::script_error("Error calling handlers");

    ActiveXBrowserHostPtr browser(getHost());
    if (!browser->isMainThread()) {
        return browser->ScheduleOnMainThread(shared_from_this(), std::bind(&IDispatchAPI::callMultipleFunctions, this, name, args, direct, ifaces));
    }

    size_t argCount(args.size());
    std::unique_ptr<CComVariant[]> comArgs(new CComVariant[argCount + 1]);
    std::unique_ptr<VARIANTARG[]> rawComArgs(new VARIANTARG[argCount + 1]);
    DISPPARAMS params;
    DISPID tid = DISPID_THIS;
    params.cArgs = args.size() + 1;
    params.cNamedArgs = 1;
    params.rgvarg = rawComArgs.get();
    params.rgdispidNamedArgs = &tid; // Needed for IE9
    
    for (size_t i = 0; i < argCount; i++) {
        browser->getComVariant(&comArgs[argCount - i], args[i]);
        // We copy w/out adding a ref so that comArgs will still clean up the values when it goes away
        rawComArgs[argCount - i] = comArgs[argCount - i];
    }
    comArgs[0] = getIDispatch(); // Needed for IE 9
    rawComArgs[0] = comArgs[0];

    CComVariant result;
    CComExcepInfo exceptionInfo;
    std::vector<FB::JSObjectPtr>::const_iterator it(direct.begin());
    std::vector<FB::JSObjectPtr>::const_iterator endit(direct.end());
    for (it; it != endit; ++it) {
        IDispatchAPIPtr ptr(std::static_pointer_cast<IDispatchAPI>(*it));
        DISPID dispId = ptr->getIDForName(std::wstring(L""));
        if (dispId == DISPID_UNKNOWN) {
             continue;
        }
        CComQIPtr<IDispatchEx> dispatchEx(ptr->getIDispatch());
        HRESULT hr;
        if (!dispatchEx) {
            hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
                DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
        } else {
            hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT,
                DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
        }
    }
    it = ifaces.begin();
    endit = ifaces.end();
    for (it; it != endit; ++it) {
        IDispatchAPIPtr ptr(std::static_pointer_cast<IDispatchAPI>(*it));
        DISPID dispId = getIDForName(FB::utf8_to_wstring(name));
        if (dispId == DISPID_UNKNOWN) {
             continue;
        }
        CComQIPtr<IDispatchEx> dispatchEx(ptr->getIDispatch());
        HRESULT hr;
        if (!dispatchEx) {
            hr = getIDispatch()->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
                DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
        } else {
            hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT,
                DISPATCH_METHOD, &params, &result, &exceptionInfo, nullptr);
        }
    }
}

FB::JSAPIPtr IDispatchAPI::getJSAPI() const
{
    if (m_browser.expired() || m_obj.expired()) {
        return FB::JSAPIPtr();
    }
    JSAPI_IDispatchExBase* p(nullptr);
    try {
        CComQIPtr<IFireBreathObject> fbObj(getIDispatch());
        // If it's our own element then both of these will pass!  This means it isn't us!
        CComQIPtr<IHTMLElement> testObj(getIDispatch());
        if (!testObj && fbObj && (p = dynamic_cast<JSAPI_IDispatchExBase*>(getIDispatch()))) {
            return p->getAPI();
        }
    } catch (...) {}

    return FB::JSAPIPtr();
}

