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

#ifndef H_IDISPATCHAPI
#define H_IDISPATCHAPI

#include "JSAPI.h"
#include "AutoPtr.h"
#include "EventHandlerObject.h"
#include "ActiveXBrowserHost.h"
#include <atlctl.h>

class IDispatchAPI :
    public FB::EventHandlerObject
{
public:
    IDispatchAPI(IDispatch *, ActiveXBrowserHost *);
    virtual ~IDispatchAPI(void);

    void *getEventId() { return (void*)m_obj; }
    void *getEventContext() { return m_browser->getContextID(); };
    IDispatch *getIDispatch() { return m_obj; }
protected:
    FB::AutoPtr<ActiveXBrowserHost> m_browser;
    CComPtr<IDispatch> m_obj;

protected:
    // Utility functions
    DISPID getIDForName(std::string name);

public:
    // Methods to query existance of members on the API
    bool HasMethod(std::string methodName);
    bool HasProperty(std::string propertyName);
    bool HasProperty(int idx);
    bool HasEvent(std::string eventName);

    // Methods to manage properties on the API
    FB::variant GetProperty(std::string propertyName);
    void SetProperty(std::string propertyName, const FB::variant value);
    FB::variant GetProperty(int idx);
    void SetProperty(int idx, const FB::variant value);

    // Methods to manage methods on the API
    FB::variant Invoke(std::string methodName, std::vector<FB::variant>& args);
};

#endif