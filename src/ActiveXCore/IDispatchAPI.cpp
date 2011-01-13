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

#include <boost/bind.hpp>
#include <dispex.h>
#include <boost/scoped_array.hpp>
#include "utf8_tools.h"
#include "JSAPI_IDispatchEx.h"
#include "axutil.h"

#include "IDispatchAPI.h"

using namespace FB::ActiveX;

FB::ActiveX::IDispatchAPI::IDispatchAPI(IDispatch *obj, const ActiveXBrowserHostPtr& host) :
    FB::JSObject(host), m_obj(obj), m_browser(host), is_JSAPI(false)
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
    host->CallOnMainThread(boost::bind(&IDispatchAPI::releaseObject, this));
}

void IDispatchAPI::releaseObject()
{
    m_obj.Release();
}

void IDispatchAPI::getMemberNames(std::vector<std::string> &nameVector) const
{
    if (!host->isMainThread()) {
        typedef void (FB::JSAPI::*getMemberNamesType)(std::vector<std::string> *nameVector) const;
        host->CallOnMainThread(boost::bind((getMemberNamesType)&FB::JSAPI::getMemberNames, this, &nameVector));
        return;
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            tmp->getMemberNames(nameVector);
        return;
    }
    HRESULT hr;
    DISPID dispid;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (!dispex) {
        throw FB::script_error("Cannot enumerate members; IDispatchEx not supported");
    }
    hr = dispex->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
    while (SUCCEEDED(hr) && dispid > -1) {
        CComBSTR curName;
        hr = dispex->GetMemberName(dispid, &curName);
        std::wstring wStr(curName);
        nameVector.push_back(FB::wstring_to_utf8(wStr));
        hr = dispex->GetNextDispID(fdexEnumAll, dispid, &dispid);
    }
}

size_t IDispatchAPI::getMemberCount() const
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&IDispatchAPI::getMemberCount, this));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->getMemberCount();
        else 
            return 0;
    }
    HRESULT hr;
    DISPID dispid;
    size_t count(0);

    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (!dispex) {
        return -1;
    }
    hr = dispex->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
    while (SUCCEEDED(hr)) {
        count++;
        hr = dispex->GetNextDispID(fdexEnumAll, dispid, &dispid);
    }
    return count;
}

DISPID IDispatchAPI::getIDForName(const std::wstring& name) const
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&IDispatchAPI::getIDForName, this, name));
    }
    if (name.empty())
        return DISPID_VALUE;
    DISPID dispId(-1);
    CW2W wcharName(name.c_str());
    OLECHAR *oleStr = wcharName;

    std::wstring wName(name);
    HRESULT hr = E_NOTIMPL;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (dispex.p != NULL) {
        hr = dispex->GetDispID(CComBSTR(name.c_str()),
            fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);
    } else {
        hr = m_obj->GetIDsOfNames(IID_NULL, &oleStr, 1, LOCALE_SYSTEM_DEFAULT, &dispId);
    }

    if (SUCCEEDED(hr)) {
        return dispId;
    } else if (hr == E_NOTIMPL) {
        return AxIdMap.getIdForValue(name); // Makes events possible
    } else {
        return -1;
    }
}

bool IDispatchAPI::HasMethod(const std::wstring& methodName) const
{
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(methodName) != -1;
}

bool IDispatchAPI::HasMethod(const std::string& methodName) const
{
    if (!host->isMainThread()) {
        typedef bool (IDispatchAPI::*curtype)(const std::string&) const;
        return host->CallOnMainThread(boost::bind((curtype)&IDispatchAPI::HasMethod, this, methodName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasMethod(methodName);
        else 
            return false;
    }
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(FB::utf8_to_wstring(methodName)) != -1 && !HasProperty(methodName);
}

bool IDispatchAPI::HasProperty(const std::wstring& propertyName) const
{
    return HasProperty(FB::wstring_to_utf8(propertyName));
}

bool IDispatchAPI::HasProperty(const std::string& propertyName) const
{
    if (!host->isMainThread()) {
        typedef bool (IDispatchAPI::*HasPropertyType)(const std::string&) const;
        return host->CallOnMainThread(boost::bind((HasPropertyType)&IDispatchAPI::HasProperty, this, propertyName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasProperty(propertyName);
        else 
            return false;
    }

    DISPPARAMS params;
    params.cArgs = 0;
    params.cNamedArgs = 0;

    VARIANT res;
    EXCEPINFO eInfo;

    HRESULT hr = E_NOTIMPL;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    DISPID id = getIDForName(FB::utf8_to_wstring(propertyName));
    if (id == -1 && propertyName != "toString")
        return false;
    // The only way to find out if the property actually exists or not is to try to get it; 
    if (dispex.p) {
        hr = dispex->InvokeEx(id, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params,
            &res, &eInfo, NULL);
    } else {
        hr = m_obj->Invoke(getIDForName(FB::utf8_to_wstring(propertyName)), IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYGET, &params, &res, NULL, NULL);
    }
    if (SUCCEEDED(hr)) {
        return true;
    } else {
        return false;
    }
}

bool IDispatchAPI::HasProperty(int idx) const
{
    FB::variant name(idx);
    return getIDForName(name.convert_cast<std::wstring>()) != -1;
}

bool IDispatchAPI::HasEvent(const std::wstring& eventName) const
{
    return getIDForName(eventName) != -1;
}

bool IDispatchAPI::HasEvent(const std::string& eventName) const
{
    if (!host->isMainThread()) {
        typedef bool (IDispatchAPI::*HasEventType)(const std::string&) const;
        return host->CallOnMainThread(boost::bind((HasEventType)&IDispatchAPI::HasEvent, this, eventName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->HasEvent(eventName);
        else 
            return false;
    }

    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(FB::utf8_to_wstring(eventName)) != -1;
}


// Methods to manage properties on the API
FB::variant IDispatchAPI::GetProperty(const std::string& propertyName)
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind((FB::GetPropertyType)&IDispatchAPI::GetProperty, this, propertyName));
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            return tmp->GetProperty(propertyName);
        else 
            return false;
    }

    DISPPARAMS params;
    params.cArgs = 0;
    params.cNamedArgs = 0;

    VARIANT res;
    EXCEPINFO eInfo;

    HRESULT hr = E_NOTIMPL;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (dispex.p) {
        hr = dispex->InvokeEx(getIDForName(FB::utf8_to_wstring(propertyName)), LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params,
            &res, &eInfo, NULL);
    } else {
        hr = m_obj->Invoke(getIDForName(FB::utf8_to_wstring(propertyName)), IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYGET, &params, &res, NULL, NULL);
    }
    if (SUCCEEDED(hr)) {
        return m_browser->getVariant(&res);
    } else {
        throw FB::script_error("Could not get property");
    }
}

void IDispatchAPI::SetProperty(const std::string& propertyName, const FB::variant& value)
{
    if (!host->isMainThread()) {
        host->CallOnMainThread(boost::bind((FB::SetPropertyType)&IDispatchAPI::SetProperty, this, propertyName, value));
        return;
    }
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            SetProperty(propertyName, value);
        return;
    }

    CComVariant arg[1];
    DISPID namedArg[1];
    DISPPARAMS params;
    params.cArgs = 1;
    params.cNamedArgs = 1;
    params.rgdispidNamedArgs = namedArg;
    params.rgvarg = arg;

    EXCEPINFO eInfo;
    VARIANT res;
    VariantInit(&res);
    m_browser->getComVariant(&arg[0], value);
    namedArg[0] = DISPID_PROPERTYPUT;

    HRESULT hr = E_NOTIMPL;
    DISPID id(getIDForName(FB::utf8_to_wstring(propertyName)));
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (dispex.p) {
        hr = dispex->InvokeEx(id, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &params,
            &res, &eInfo, NULL);
    } else {
        hr = m_obj->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYPUT, &params, &res, NULL, NULL);
    }

    if (!SUCCEEDED(hr)) {
        throw FB::script_error("Could not set property");
    }
}

FB::variant IDispatchAPI::GetProperty(int idx)
{
    FB::variant sIdx(idx);
    return GetProperty(sIdx.convert_cast<std::string>());
}

void IDispatchAPI::SetProperty(int idx, const FB::variant& value)
{
    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
        if (tmp)
            SetProperty(idx, value);
    }
    FB::variant sIdx(idx);
    SetProperty(sIdx.convert_cast<std::string>(), value);
}


// Methods to manage methods on the API
FB::variant IDispatchAPI::Invoke(const std::string& methodName, const std::vector<FB::variant>& args)
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind((FB::InvokeType)&IDispatchAPI::Invoke, this, methodName, args));
    }

    boost::scoped_array<CComVariant> comArgs(new CComVariant[args.size()]);
    DISPPARAMS params;
    params.cArgs = args.size();
    params.cNamedArgs = 0;
    params.rgvarg = comArgs.get();

    VARIANT res;
    for (size_t i = 0; i < args.size(); i++) {
        m_browser->getComVariant(&comArgs[args.size() - 1 - i], args[i]);
    }

    HRESULT hr = m_obj->Invoke(getIDForName(FB::utf8_to_wstring(methodName)), IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_METHOD, &params, &res, NULL, NULL);

    if (SUCCEEDED(hr)) {
        return m_browser->getVariant(&res);
    } else {
        throw FB::script_error("Could not get property");
    }
}

FB::JSObjectPtr IDispatchAPI::Construct( const std::string& memberName, const FB::VariantList& args )
{
    return FB::JSObjectPtr();
}

FB::JSAPIPtr IDispatchAPI::getJSAPI() const
{
    if (!m_obj) {
        return FB::JSAPIPtr();
    }
    JSAPI_IDispatchExBase* p(NULL);
    CComQIPtr<IFireBreathObject> fbObj(m_obj);
    // If it's our own element then both of these will pass!  This means it isn't us!
    CComQIPtr<IHTMLElement> testObj(m_obj);
    if (!testObj && fbObj && (p = dynamic_cast<JSAPI_IDispatchExBase*>(m_obj.p))) {
        return p->getAPI();
    }

    return FB::JSAPIPtr();
}
