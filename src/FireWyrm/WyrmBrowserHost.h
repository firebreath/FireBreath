/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 9, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_WYRMBROWSERHOST
#define H_WYRMBROWSERHOST

#include "FireWyrm.h"
#include "BrowserHost.h"
#include "SafeQueue.h"
#include "ShareableReference.h"
#include <boost/thread.hpp>

namespace FB {
    namespace FireWyrm {

    FB_FORWARD_PTR(WyrmColony);
    FB_FORWARD_PTR(AlienWyrmling);
    FB_FORWARD_PTR(WyrmBrowserHost);
    FB_FORWARD_PTR(WyrmJavascriptObject);
    class LocalWyrmling;
        
    using WyrmlingKey = std::pair<FW_INST, FW_INST>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  WyrmBrowserHost
    ///
    /// @brief  Provides a FB::BrowserHost implementation for Wyrm
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class WyrmBrowserHost :
        public FB::BrowserHost
    {
    public:
        WyrmBrowserHost(WyrmColony *module, const FW_INST spawnId);
        Promise<void> init();
        virtual ~WyrmBrowserHost(void);

    public:
        virtual BrowserStreamPtr _createStream( const BrowserStreamRequest& req ) const override;

        virtual BrowserStreamPtr _createUnsolicitedStream(const BrowserStreamRequest& req) const override;

    public:
        virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const override;
        virtual void *getContextID() const override { return (void *)(m_spawnId); }
        virtual FW_INST getSpawnId() const { return m_spawnId; }

    public:
        int delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
                const FB::VariantList& args, std::string fname = "") override;
        void DoDeferredRelease() const override;
        FB::DOM::DocumentPtr getDOMDocument() override;
        FB::DOM::WindowPtr getDOMWindow() override;
        FB::DOM::ElementPtr getDOMElement() override;
        void evaluateJavaScript(const std::string &script) override;

        virtual Promise<FB::VariantList> GetArrayValues(FB::JSObjectPtr obj) override;
        virtual Promise<FB::VariantMap> GetObjectValues(FB::JSObjectPtr obj) override;

        virtual bool DetectProxySettings(std::map<std::string, std::string>& settingsMap, std::string url = "") override;

    public:
        void shutdown() override;

        LocalWyrmling getWyrmling(FB::JSAPIPtr api);
        LocalWyrmling getWyrmling(FB::JSAPIWeakPtr api);
        LocalWyrmling getWyrmling(FW_INST objId);
        LocalWyrmling createWyrmling(FB::JSAPIPtr api, FW_INST objId);
        LocalWyrmling createWyrmling(FB::JSAPIWeakPtr api, FW_INST objId);
        
        FB::JSAPIPtr getJSAPIFromWyrmling(FW_INST objId);

        bool HasMethod(FW_INST objId, std::string name);
        
        // Wyrm Scripting methods
        FB::VariantListPromise Enum(FW_INST objId);
        FB::variantPromise Invoke(FW_INST objId, std::string name, FB::VariantList args);
        FB::variantPromise GetP(FW_INST objId, std::string name);
        FB::Promise<void> SetP(FW_INST objId, std::string name, FB::variant value);
        FB::Promise<void> RelObj(FW_INST objId);

        FB::variantPromise DoCommand(FB::VariantList cmd);

    protected:
        WyrmColony *module;
        FW_INST m_spawnId;
        FW_INST m_nextObjId;
        FW_INST m_browserObjId;
        AlienWyrmlingPtr m_Browser;
        Promise<void> m_onReady;
        JSObjectPtr m_DOMWindow;
        JSObjectPtr m_DOMDocument;

        std::map<FW_INST, LocalWyrmling> m_localMap;
    };
}; };

#endif


