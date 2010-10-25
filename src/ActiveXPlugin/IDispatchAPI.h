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

#pragma once
#ifndef H_IDISPATCHAPI
#define H_IDISPATCHAPI

#include "Win/win_common.h"
#include "JSAPI.h"
#include "JSObject.h"
#include "ActiveXBrowserHost.h"
#include <atlctl.h>
#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class  IDispatchAPI
///
/// @brief  Provide a JSObject implementation to wrap a IDispatch ActiveX object
////////////////////////////////////////////////////////////////////////////////////////////////////
class IDispatchAPI :
    public FB::JSObject
{
public:
    IDispatchAPI(IDispatch *, ActiveXBrowserHostPtr);
    virtual ~IDispatchAPI(void);

    void *getEventId() { return (void*)m_obj; }
    void *getEventContext() { return m_browser->getContextID(); };
    IDispatch *getIDispatch() { return m_obj; }

    // Enumerate members
    void getMemberNames(std::vector<std::string> &nameVector);
    size_t getMemberCount();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief  Releases the object.  Do not call this method, it is used by the destructor, but has
    /// 		to be public so that it can be forced to run on the main thread.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void releaseObject();

protected:
    ActiveXBrowserHostPtr m_browser;
    CComQIPtr<IDispatch, &IID_IDispatch> m_obj;

protected:
    DISPID getIDForName(const std::wstring& name);

public:
    bool HasMethod(const std::string& methodName);
    bool HasMethod(const std::wstring& methodName);
    bool HasProperty(const std::string& propertyName);
    bool HasProperty(const std::wstring& propertyName);
    bool HasProperty(int idx);
    bool HasEvent(const std::string& eventName);
    bool HasEvent(const std::wstring& eventName);

    FB::variant GetProperty(const std::string& propertyName);
    void SetProperty(const std::string& propertyName, const FB::variant& value);
    FB::variant GetProperty(int idx);
    void SetProperty(int idx, const FB::variant& value);

    FB::variant Invoke(const std::string& methodName, const std::vector<FB::variant>& args);
};

#endif
