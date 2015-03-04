/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPOBJECTAPI
#define H_NPOBJECTAPI

#include "NpapiTypes.h"
#include "npruntime.h"
#include "JSAPI.h"
#include "JSObject.h"
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  NPObjectAPI
    ///
    /// @brief  Provides a FB::JSObject implementation that wraps a NPObject*
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class NPObjectAPI :
        public FB::JSObject
    {
    public:
        NPObjectAPI(NPObject *, const NpapiBrowserHostPtr&);
        virtual ~NPObjectAPI(void);

        void *getEventId() const { return (void*)obj; }
        void *getEventContext() const {
            if (!m_browser.expired())
                return getHost()->getContextID();
            else
                return NULL;
        };
        NPObject *getNPObject() const { return obj; }

        void getMemberNames(std::vector<std::string> &nameVector) const;
        size_t getMemberCount() const;
    public:
        void invalidate() { }
        bool isValid() { return !m_browser.expired(); }
        virtual bool supportsOptimizedCalls() const { return true; }
        virtual void callMultipleFunctions(std::string name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces);

    protected:
        NpapiBrowserHostPtr getHost() const {
            NpapiBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                throw std::bad_cast();
            }
            return ptr;
        }
        NpapiBrowserHostWeakPtr m_browser;
        NPObject *obj;

    public:
        // The methods below are not to be used outside of the NPAPI part of the FireBreath core;
        // doing so basically undermines the abstraction and makes your plugin platform-specific

        variant GetPropertySync(std::string propertyName);
        void SetPropertySync(std::string propertyName, const variant& value);
        void RemovePropertySync(std::string propertyName);
        variant GetPropertySync(int idx);
        void SetPropertySync(int idx, const variant& value);
        void RemovePropertySync(int idx);

        variant InvokeSync(std::string methodName, const std::vector<variant>& args);

        bool HasMethod(std::string methodName) const override;
        bool HasProperty(std::string propertyName) const override;
        bool HasProperty(int idx) const override;

    public:
        variantPromise GetProperty(std::string propertyName) override;
        void SetProperty(std::string propertyName, const variant& value) override;
        void RemoveProperty(std::string propertyName) override;
        variantPromise GetProperty(int idx) override;
        void SetProperty(int idx, const variant& value) override;
        void RemoveProperty(int idx) override;

        variantPromise Invoke(std::string methodName, const std::vector<variant>& args) override;

        friend class NpapiBrowserHost;
    };

}; };

#endif

