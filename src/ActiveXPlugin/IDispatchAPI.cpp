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

IDispatchAPI::IDispatchAPI(IDispatch *obj, ActiveXBrowserHost *host) : 
    m_obj(obj), m_browser(host), FB::BrowserObjectAPI(host)
{
}

IDispatchAPI::~IDispatchAPI(void)
{
}

DISPID IDispatchAPI::getIDForName(std::string name)
{
    USES_CONVERSION;
    CA2W wStr(name.c_str());
    OLECHAR *oleStr = wStr;
    DISPID dispId(-1);
    HRESULT hr = m_obj->GetIDsOfNames(IID_NULL, &oleStr, 1, LOCALE_SYSTEM_DEFAULT, &dispId);
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

    HRESULT hr = m_obj->Invoke(getIDForName(propertyName), IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_PROPERTYGET, &params, &res, NULL, NULL);

    if (SUCCEEDED(hr)) {
        return m_browser->getVariant(&res);
    } else {
        throw FB::script_error("Could not get property");
    }
}

void IDispatchAPI::SetProperty(std::string propertyName, const FB::variant value)
{
    CComVariant arg[1];
    DISPPARAMS params;
    params.cArgs = 1;
    params.cNamedArgs = 0;
    params.rgvarg = arg;

    m_browser->getComVariant(&arg[0], value);

    HRESULT hr = m_obj->Invoke(getIDForName(propertyName), IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_PROPERTYPUT, &params, NULL, NULL, NULL);

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