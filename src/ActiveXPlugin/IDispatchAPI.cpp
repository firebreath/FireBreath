/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 4, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
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

    hr = m_obj->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
    while (SUCCEEDED(hr)) {
        CComBSTR curName;
        hr = m_obj->GetMemberName(dispid, &curName);
        CW2A cStr(curName);
        nameVector.push_back(std::string(cStr));
        hr = m_obj->GetNextDispID(fdexEnumAll, dispid, &dispid);
    }
}

size_t IDispatchAPI::getMemberCount()
{
    HRESULT hr;
    DISPID dispid;
    size_t count(0);

    hr = m_obj->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
    while (SUCCEEDED(hr)) {
        count++;
        hr = m_obj->GetNextDispID(fdexEnumAll, dispid, &dispid);
    }
    return count;
}

DISPID IDispatchAPI::getIDForName(std::string name)
{
    USES_CONVERSION;
    CA2W wStr(name.c_str());
    OLECHAR *oleStr = wStr;
    DISPID dispId(-1);

    HRESULT hr = E_NOTIMPL;
    hr = m_obj->GetDispID(CComBSTR(name.c_str()),
        fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);

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
    hr = m_obj->InvokeEx(getIDForName(propertyName), LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
        &params, &res, &eInfo, NULL);

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
    hr = m_obj->InvokeEx(id, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &params,
        &res, &eInfo, NULL);

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
    for (int i = args.size() - 1; i >= 0; i--) {
        m_browser->getComVariant(&comArgs[i], args[i]);
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