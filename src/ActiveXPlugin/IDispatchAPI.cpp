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

#include "IDispatchAPI.h"
#include "axmain.h"
#include <dispex.h>

IDispatchAPI::IDispatchAPI(IDispatch *obj, ActiveXBrowserHost *host) :
    m_obj(obj), m_browser(host), FB::BrowserObjectAPI(host)
{
}

IDispatchAPI::~IDispatchAPI(void)
{
}

void IDispatchAPI::getMemberNames(std::vector<std::string> &nameVector)
{
    USES_CONVERSION;
    HRESULT hr;
    DISPID dispid;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (!dispex) {
        throw FB::script_error("Cannot enumerate members; IDispatchEx not supported");
    }
    hr = dispex->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
    while (SUCCEEDED(hr)) {
        CComBSTR curName;
        hr = dispex->GetMemberName(dispid, &curName);
        CW2A cStr(curName);
        nameVector.push_back(std::string(cStr));
        hr = dispex->GetNextDispID(fdexEnumAll, dispid, &dispid);
    }
}

size_t IDispatchAPI::getMemberCount()
{
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

DISPID IDispatchAPI::getIDForName(std::string name)
{
    if (name.empty())
        return DISPID_VALUE;
    USES_CONVERSION;
    CA2W wStr(name.c_str());
    OLECHAR *oleStr = wStr;
    DISPID dispId(-1);

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

bool IDispatchAPI::HasMethod(std::string methodName)
{
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(methodName) != -1;
}

bool IDispatchAPI::HasProperty(std::string propertyName)
{
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(propertyName) != -1;
}

bool IDispatchAPI::HasProperty(int idx)
{
    FB::variant name(idx);
    return getIDForName(name.convert_cast<std::string>()) != -1;
}

bool IDispatchAPI::HasEvent(std::string eventName)
{
    // This will actually just return true if the specified member exists; IDispatch doesn't really
    // differentiate further than that
    return getIDForName(eventName) != -1;
}


// Methods to manage properties on the API
FB::variant IDispatchAPI::GetProperty(std::string propertyName)
{
    DISPPARAMS params;
    params.cArgs = 0;
    params.cNamedArgs = 0;

    VARIANT res;
    EXCEPINFO eInfo;

    HRESULT hr = E_NOTIMPL;
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (dispex.p) {
        hr = dispex->InvokeEx(getIDForName(propertyName), LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params,
            &res, &eInfo, NULL);
    } else {
        hr = m_obj->Invoke(getIDForName(propertyName), IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_PROPERTYGET, &params, &res, NULL, NULL);
    }
    if (SUCCEEDED(hr)) {
        return m_browser->getVariant(&res);
    } else {
        throw FB::script_error("Could not get property");
    }
}

void IDispatchAPI::SetProperty(std::string propertyName, const FB::variant value)
{
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
    DISPID id(getIDForName(propertyName));
    CComQIPtr<IDispatchEx, &IID_IDispatchEx> dispex(m_obj);
    if (dispex.p) {
        hr = dispex->InvokeEx(id, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &params,
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

void IDispatchAPI::SetProperty(int idx, const FB::variant value)
{
    FB::variant sIdx(idx);
    SetProperty(sIdx.convert_cast<std::string>(), value);
}


// Methods to manage methods on the API
FB::variant IDispatchAPI::Invoke(std::string methodName, std::vector<FB::variant>& args)
{
    CComVariant *comArgs = new CComVariant[args.size()];
    DISPPARAMS params;
    params.cArgs = args.size();
    params.cNamedArgs = 0;
    params.rgvarg = comArgs;

    VARIANT res;
    for (size_t i = 0; i < args.size(); i++) {
        m_browser->getComVariant(&comArgs[args.size() - 1 - i], args[i]);
    }

    HRESULT hr = m_obj->Invoke(getIDForName(methodName), IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_METHOD, &params, &res, NULL, NULL);

    if (SUCCEEDED(hr)) {
        delete [] comArgs;
        return m_browser->getVariant(&res);
    } else {
        throw FB::script_error("Could not get property");
    }
}
