/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 16, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FWALIENWYRMLING
#define H_FWALIENWYRMLING

#include "APITypes.h"
#include "JSAPI.h"
#include "JSFunction.h"
#include "ShareableReference.h"
#include "WyrmBrowserHost.h"
#include <memory>

namespace FB { namespace FireWyrm {
    FB_FORWARD_PTR(AlienWyrmling);
    FB_FORWARD_PTR(AlienLarvae);
    class WyrmColony;

    class AlienLarvae : public std::enable_shared_from_this<AlienLarvae>
    {
    public:
        AlienLarvae(WyrmColony* colony, FW_INST spawnId, uint32_t objId);
        ~AlienLarvae();

    private:
        friend class AlienWyrmling;
        WyrmColony* getColony() { return colony; }
        WyrmColony* colony;
        FW_INST spawnId;
        uint32_t objId;
        FB::Promise< std::vector< std::string > > m_membersDfd;
    };

    class AlienWyrmling :
        public FB::JSObject
    {
    private:
        struct PrivateStruct {};
    public:
        static AlienWyrmlingPtr create(const WyrmBrowserHostPtr h, const AlienLarvaePtr larvae);
        AlienWyrmling(const WyrmBrowserHostPtr h, PrivateStruct);
        void init(AlienLarvaePtr ptr);
        virtual ~AlienWyrmling(void);

        void *getEventId() const { return (void*)m_objId; }
        void *getEventContext() const {
            if (!m_browser.expired())
                return getHost()->getContextID();
            else
                return nullptr;
        };
        uint32_t getObjectId() { return m_objId; }
        uint32_t getSpawnId() { return m_spawnId; }
        WyrmlingKey getWyrmlingKey() { return WyrmlingKey(getSpawnId(), getObjectId()); }

        void getMemberNames(std::vector<std::string> &nameVector) const {
            nameVector.insert(nameVector.begin(), m_memberNames.begin(), m_memberNames.end());
        }
        size_t getMemberCount() const {
            return m_memberNames.size();
        }
    public:
        void invalidate();
        bool isValid() { return !m_browser.expired() && m_valid; }
        virtual bool supportsOptimizedCalls() const { return true; }
        virtual void callMultipleFunctions(std::string name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces);

        virtual bool HasMethod(std::string methodName) const override {
            return HasProperty(methodName);
        }
        virtual bool HasProperty(std::string propertyName) const override {
            return std::find(m_memberNames.begin(), m_memberNames.end(), propertyName) != m_memberNames.end();
        }
        virtual bool HasProperty(int idx) const override {
            return HasProperty(std::to_string(idx));
        }
    private:
        WyrmBrowserHostPtr getHost() const {
            WyrmBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                throw std::bad_cast();
            }
            return ptr;
        }
        WyrmBrowserHostWeakPtr m_browser;
        FB::Promise<void> m_startup;
        bool m_valid;
        std::vector<std::string> m_memberNames;
        FW_INST m_spawnId;
        uint32_t m_objId;

    public:
        variantPromise GetProperty(std::string propertyName) override;
        void SetProperty(std::string propertyName, const variant& value) override;
        void RemoveProperty(std::string propertyName) override;
        variantPromise GetProperty(int idx) override {
            return GetProperty(std::to_string(idx));
        }
        void SetProperty(int idx, const variant& value) override {
            return SetProperty(std::to_string(idx), value);
        }
        void RemoveProperty(int idx) override {
            return RemoveProperty(std::to_string(idx));
        }

        variantPromise Invoke(std::string methodName, const VariantList& args) override;
    };

}; };

#endif // H_FWALIENWYRMLING