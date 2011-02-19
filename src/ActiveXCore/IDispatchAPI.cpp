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
#include <boost/bind.hpp>
#include <dispex.h>
#include <boost/scoped_array.hpp>
#include "utf8_tools.h"
#include "JSAPI_IDispatchEx.h"
#include "axutil.h"

#include "IDispatchAPI.h"

using namespace FB::ActiveX;

boost::shared_ptr<FB::ActiveX::IDispatchAPI> IDispatchAPI::create(IDispatch * obj, const ActiveXBrowserHostPtr& host)
{
	return boost::make_shared<IDispatchAPI>(obj, host);
}

FB::ActiveX::IDispatchAPI::IDispatchAPI(IDispatch * obj, const ActiveXBrowserHostPtr& host) :
    FB::JSObject(host), m_obj(obj), m_browser(host), is_JSAPI(false)
{
    m_obj->AddRef();
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
    m_browser->deferred_release(m_obj);
    m_obj = NULL;
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

    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
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
			nameVector.push_back(FB::wstring_to_utf8(name));
		}
	}
}

size_t IDispatchAPI::getMemberCount() const
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&IDispatchAPI::getMemberCount, this));
    }

    if (is_JSAPI) {
        FB::JSAPIPtr tmp = inner.lock();
		if (!tmp) {
			// TODO: check if this should be -1
			return 0;
		}
		return tmp->getMemberCount();
    }

    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
    if (!dispatchEx) {
        return -1;
    }

    size_t count = 0;
    DISPID dispid = DISPID_STARTENUM;    
    while (dispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid) != S_FALSE) {
        if (dispid >= 0) {
			++count;
		}
    }

    return count;
}

DISPID IDispatchAPI::getIDForName(const std::wstring& name) const
{
    if (!host->isMainThread()) {
        return host->CallOnMainThread(boost::bind(&IDispatchAPI::getIDForName, this, name));
    }

	if (name.empty()) {
        return DISPID_VALUE;
	}

	HRESULT hr = E_NOTIMPL;
    DISPID dispId = DISPID_UNKNOWN;
    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
    if (dispatchEx) {
        hr = dispatchEx->GetDispID(CComBSTR(name.c_str()),
            fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);
    } else {
		const wchar_t* p = name.c_str();
        hr = m_obj->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&p), 1, LOCALE_SYSTEM_DEFAULT, &dispId);
    }

	if (FAILED(hr)) {
		if (hr == E_NOTIMPL) {
			return AxIdMap.getIdForValue(name); // Makes events possible
		}
		return DISPID_UNKNOWN;
	}
	
	return dispId;
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

bool IDispatchAPI::HasProperty(const std::string& propertyName) const
{
    if (!host->isMainThread()) {
        typedef bool (IDispatchAPI::*HasPropertyType)(const std::string&) const;
        return host->CallOnMainThread(boost::bind((HasPropertyType)&IDispatchAPI::HasProperty, this, propertyName));
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
    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
    if (dispatchEx) {
        hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, 
			DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, NULL);
    } else {
        hr = m_obj->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, NULL);
    }

	return SUCCEEDED(hr);
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
		if (!tmp) {
			return false;
		}
		return tmp->HasEvent(eventName);
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
		if (!tmp) {
            return false;
		}
		return tmp->GetProperty(propertyName);
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
    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
    if (dispatchEx) {
        hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params,
            &result, &exceptionInfo, NULL);
    } else {
        hr = m_obj->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, NULL);
    }
    
	if (FAILED(hr)) {
        throw FB::script_error("Could not get property");
    }
	
	return m_browser->getVariant(&result);
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

    m_browser->getComVariant(&arg[0], value);
    rawArg[0] = arg[0];
    namedArg[0] = DISPID_PROPERTYPUT;

    HRESULT hr;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    CComQIPtr<IDispatchEx> dispatchEx(m_obj);
    if (dispatchEx) {
        hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &params,
            &result, &exceptionInfo, NULL);
    } else {
        hr = m_obj->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYPUT, &params, &result, &exceptionInfo, NULL);
    }

    if (FAILED(hr)) {
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

	DISPID dispId = getIDForName(FB::utf8_to_wstring(methodName));
	if (dispId == DISPID_UNKNOWN) {
		 throw FB::script_error("Method invoke failed");
	}

    size_t argCount(args.size());
    boost::scoped_array<CComVariant> comArgs(new CComVariant[argCount]);
    boost::scoped_array<VARIANTARG> rawComArgs(new VARIANTARG[argCount]);
    DISPPARAMS params;
    params.cArgs = args.size();
    params.cNamedArgs = 0;
    params.rgvarg = rawComArgs.get();

    for (size_t i = 0; i < args.size(); i++) {
        m_browser->getComVariant(&comArgs[argCount - 1 - i], args[i]);
        // We copy w/out adding a ref so that comArgs will still clean up the values when it goes away
        rawComArgs[argCount - 1 - i] = comArgs[argCount - 1 - i];
    }

    CComVariant result;
	CComExcepInfo exceptionInfo;
    HRESULT hr = m_obj->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_METHOD, &params, &result, &exceptionInfo, NULL);
    if (FAILED(hr)) {
        throw FB::script_error("Method invoke failed");
    }
	
	return m_browser->getVariant(&result);
}

//FB::JSObjectPtr IDispatchAPI::Construct( const std::string& memberName, const FB::VariantList& args )
//{
//    return FB::JSObjectPtr();
//}

FB::JSAPIPtr IDispatchAPI::getJSAPI() const
{
    if (!m_obj) {
        return FB::JSAPIPtr();
    }
    JSAPI_IDispatchExBase* p(NULL);
    CComQIPtr<IFireBreathObject> fbObj(m_obj);
    // If it's our own element then both of these will pass!  This means it isn't us!
    CComQIPtr<IHTMLElement> testObj(m_obj);
    if (!testObj && fbObj && (p = dynamic_cast<JSAPI_IDispatchExBase*>(m_obj))) {
        return p->getAPI();
    }

    return FB::JSAPIPtr();
}

