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
    FB_FORWARD_PTR(WyrmBrowserHost);
    FB_FORWARD_PTR(WyrmJavascriptObject);

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
        virtual ~WyrmBrowserHost(void);

    public:
        virtual BrowserStreamPtr _createStream( const BrowserStreamRequest& req ) const;

        virtual BrowserStreamPtr _createUnsolicitedStream(const BrowserStreamRequest& req) const;

    public:
        virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const;
        virtual void *getContextID() const { return (void *)m_spawnId; }
        virtual FW_INST getSpawnId() const { return m_spawnId; }

    public:
        int delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
                const FB::VariantList& args, std::string fname = "") override;
        void DoDeferredRelease() const override;
        FB::DOM::DocumentPtr getDOMDocument() override;
        FB::DOM::WindowPtr getDOMWindow() override;
        FB::DOM::ElementPtr getDOMElement() override;
        void evaluateJavaScript(const std::string &script) override;

        virtual Promise<FB::VariantList> GetArrayValues(FB::JSObjectPtr obj);
        virtual Promise<FB::VariantMap> GetObjectValues(FB::JSObjectPtr obj);

        virtual bool DetectProxySettings(std::map<std::string, std::string>& settingsMap, const std::string& url = "");

    public:
        void shutdown();

    // NPN_ functions -- for scope reasons, we no longer access these using the global functions
    protected:
        WyrmColony *module;
        FW_INST m_spawnId;
        FW_INST m_nextObjId;
        //NPObjectAPIPtr m_htmlDoc;
        //NPObjectAPIPtr m_htmlWin;
        //NPObjectAPIPtr m_htmlElement;
    };
}; };

#endif


