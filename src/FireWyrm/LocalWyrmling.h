/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FWLOCALWYRMLING
#define H_FWLOCALWYRMLING

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#include "APITypes.h"
#include "JSAPI.h"
#include "JSFunction.h"
#include "ShareableReference.h"
#include "WyrmBrowserHost.h"

namespace FB { namespace FireWyrm {
    class LocalWyrmling
    {
    private:
        WyrmBrowserHostPtr getHost() {
            WyrmBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                Invalidate();
                throw std::runtime_error("The object is not valid");
            }
            return ptr;
        }
        FB::JSAPIWeakPtr m_api;
        WyrmBrowserHostWeakPtr m_browser;
        bool m_valid;
        bool m_autoRelease;
        FW_INST m_id;

    public:
        LocalWyrmling() : m_valid(false), m_autoRelease(false) {}
        LocalWyrmling(const WyrmBrowserHostPtr& host, const FB::JSAPIWeakPtr& api, FW_INST id, bool autoRelease = false);

        // Allow copying and moving of this object
        LocalWyrmling(const LocalWyrmling &rh);
        LocalWyrmling& operator=(const LocalWyrmling&rh);
        LocalWyrmling(LocalWyrmling &&rh);

    private:
        void swap(LocalWyrmling& s) NOEXCEPT;
    public:
        
        FB::JSAPIPtr getAPI() const
        {
            FB::JSAPIPtr ptr(m_api.lock());
            if (!ptr)
                throw std::bad_cast();
            return ptr;
        }
        bool containsAPI(FB::JSAPIPtr api) const {
            return m_api.lock() == getAPI();
        }
        FW_INST getObjectId() const { return m_id; }
        virtual ~LocalWyrmling(void);

        void Invalidate() {
            m_valid = false;
            m_api.reset();
            m_browser.reset();
        }
        bool isValid() {
            return m_valid && !m_api.expired() && !m_browser.expired();
        }

        FB::VariantListPromise Enum();
        FB::variantPromise Invoke(std::string name, FB::VariantList args);
        FB::variantPromise GetP(std::string name);
        FB::Promise<void> SetP(std::string name, FB::variant value);
        
        bool HasMethod(std::string propName);

    private:
        FB::variant addEventListener(const std::vector<variant> args);
        FB::variant removeEventListener(const std::vector<variant> args);
    };

}; };

#endif
