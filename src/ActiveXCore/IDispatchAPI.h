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

#include "win_common.h"
#include "JSAPI.h"
#include "JSObject.h"
#include "ActiveXBrowserHost.h"
#include <atlctl.h>
#include <vector>
#include <string>

namespace FB { namespace ActiveX {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  IDispatchAPI
    ///
    /// @brief  Provide a JSObject implementation to wrap a IDispatch ActiveX object
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IDispatchAPI :
        public FB::JSObject
    {
    public:
        static boost::shared_ptr<IDispatchAPI> create(IDispatch *, const ActiveXBrowserHostPtr&);
        virtual ~IDispatchAPI(void);

        void *getEventId() const { return (void*)m_obj; }
        void *getEventContext() const { return m_browser->getContextID(); };
        IDispatch *getIDispatch() const { return m_obj; }

        // Enumerate members
        void getMemberNames(std::vector<std::string> &nameVector) const;
        size_t getMemberCount() const;

    protected:
        ActiveXBrowserHostPtr m_browser;
        IDispatch* m_obj;
        bool is_JSAPI;
        FB::JSAPIWeakPtr inner;

    protected:
        DISPID getIDForName(const std::wstring& name) const;

    public:
        bool HasMethod(const std::string& methodName) const;
        bool HasMethod(const std::wstring& methodName) const;
        bool HasProperty(const std::string& propertyName) const;
        bool HasProperty(const std::wstring& propertyName) const;
        bool HasProperty(int idx) const;
        bool HasEvent(const std::string& eventName) const;
        bool HasEvent(const std::wstring& eventName) const;

        FB::variant GetProperty(const std::string& propertyName);
        void SetProperty(const std::string& propertyName, const FB::variant& value);
        FB::variant GetProperty(int idx);
        void SetProperty(int idx, const FB::variant& value);

        FB::variant Invoke(const std::string& methodName, const FB::VariantList& args);
        FB::JSObjectPtr Construct(const std::string& memberName, const FB::VariantList& args);

    public:
        virtual FB::JSAPIPtr getJSAPI() const;

    private:
        friend boost::shared_ptr<IDispatchAPI> boost::make_shared<IDispatchAPI>(IDispatch * const &, const ActiveXBrowserHostPtr&);
        IDispatchAPI(IDispatch *, const ActiveXBrowserHostPtr&);
    };
} }
#endif

