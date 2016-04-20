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
        static std::shared_ptr<IDispatchAPI> create(IDispatch *, const ActiveXBrowserHostPtr&);
        virtual ~IDispatchAPI(void);

        void *getEventId() const
        {
            IDispatchSRef sref(m_obj.lock());
            if (sref) return (void*)sref->getPtr();
            else return nullptr;
        }
        void *getEventContext() const {
            if (m_browser.expired()) return nullptr;
            else return getHost()->getContextID();
        };
        IDispatch* getIDispatch() const {
            IDispatchSRef sref(m_obj);
            return sref->getPtr();
        }
        bool isValid() { return !m_obj.expired() && !m_browser.expired(); }
        virtual bool supportsOptimizedCalls() const { return true; }
        virtual void callMultipleFunctions(std::string name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces);
        void invalidate() { m_obj.reset(); }

        // Enumerate members
        void getMemberNames(std::vector<std::string> &nameVector) const;
        size_t getMemberCount() const;

    protected:
        ActiveXBrowserHostPtr getHost() const {
            ActiveXBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                throw FB::bad_cast_ext("BrowserHost has shut down");
            }
            return ptr;
        }
        ActiveXBrowserHostWeakPtr m_browser;
        IDispatchWRef m_obj;
        bool is_JSAPI;
        FB::JSAPIWeakPtr inner;

        DISPID getIDForName(const std::wstring& name) const;

    public:
        // Seriously, don't call these methods from your plugin -- it makes things totally platform-specific
        FB::variant GetPropertySync(std::string propertyName);
        FB::variant GetPropertySync(int idx);
        FB::variant InvokeSync(std::string methodName, const FB::VariantList& args);

    public:
        bool HasMethod(std::string methodName) const override;
        bool HasMethod(const std::wstring& methodName) const override;
        bool HasProperty(std::string propertyName) const override;
        bool HasProperty(const std::wstring& propertyName) const override;
        bool HasProperty(int idx) const override;

        FB::variantPromise GetProperty(std::string propertyName) override;
        void SetProperty(std::string propertyName, const FB::variant& value) override;
        void RemoveProperty(std::string propertyName) override;
        FB::variantPromise GetProperty(int idx) override;
        void SetProperty(int idx, const FB::variant& value) override;
        void RemoveProperty(int idx) override;

        FB::variantPromise Invoke(std::string methodName, const FB::VariantList& args) override;

    public:
        virtual FB::JSAPIPtr getJSAPI() const;

    private:
        IDispatchAPI(IDispatch *, const ActiveXBrowserHostPtr&);

        friend class ActiveXBrowserHost;
    };
} }
#endif

