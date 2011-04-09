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
        virtual JSAPIPtr getJSAPI() const;
        void invalidate() { inner.reset(); }
        bool isValid() { return !m_browser.expired(); }
        virtual bool supportsOptimizedCalls() const { return true; }
        virtual void callMultipleFunctions(const std::string& name, const FB::VariantList& args,
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
        bool is_JSAPI;
        FB::JSAPIWeakPtr inner;

    public:
        bool HasMethod(const std::string& methodName) const;
        bool HasProperty(const std::string& propertyName) const;
        bool HasProperty(int idx) const;

        variant GetProperty(const std::string& propertyName);
        void SetProperty(const std::string& propertyName, const variant& value);
        void RemoveProperty(const std::string& propertyName);
        variant GetProperty(int idx);
        void SetProperty(int idx, const variant& value);
        void RemoveProperty(int idx);

        variant Invoke(const std::string& methodName, const std::vector<variant>& args);

        variant Construct(const std::vector<variant>& args);
    };

}; };

#endif

