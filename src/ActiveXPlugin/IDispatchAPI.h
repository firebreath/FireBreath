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

#ifndef H_IDISPATCHAPI
#define H_IDISPATCHAPI

#include "Win/win_common.h"
#include "JSAPI.h"
#include "AutoPtr.h"
#include "BrowserObjectAPI.h"
#include "ActiveXBrowserHost.h"
#include <atlctl.h>
#include <vector>
#include <string>

class IDispatchAPI :
    public FB::BrowserObjectAPI
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

protected:
    ActiveXBrowserHostPtr m_browser;
    CComQIPtr<IDispatch, &IID_IDispatch> m_obj;

protected:
    // Utility functions
    DISPID getIDForName(const std::string& name);

public:
    // Methods to query existance of members on the API
    bool HasMethod(const std::string& methodName);
    bool HasProperty(const std::string& propertyName);
    bool HasProperty(int idx);
    bool HasEvent(const std::string& eventName);

    // Methods to manage properties on the API
    FB::variant GetProperty(const std::string& propertyName);
    void SetProperty(const std::string& propertyName, const FB::variant& value);
    FB::variant GetProperty(int idx);
    void SetProperty(int idx, const FB::variant& value);

    // Methods to manage methods on the API
    FB::variant Invoke(const std::string& methodName, const std::vector<FB::variant>& args);
};

#endif