/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_ACTIVEXBROWSERHOST
#define H_ACTIVEXBROWSERHOST

#include "win_common.h"
//#include "global/COM_config.h"
#include <atlctl.h>
#include <map>
#include "BrowserHost.h"
#include "APITypes.h"
#include "FBPointers.h"
#include "SafeQueue.h"
#include "ShareableReference.h"
#include "ActiveXFactoryDefinitions.h"

namespace FB {
    class WinMessageWindow;
    class BrowserStreamRequest;
    namespace ActiveX {
        FB_FORWARD_PTR(ActiveXBrowserHost);
        FB_FORWARD_PTR(IDispatchAPI);

        using IDispatchWRef = std::weak_ptr<FB::ShareableReference<IDispatch> >;
        using IDispatchSRef = std::shared_ptr<FB::ShareableReference<IDispatch> >;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXBrowserHost
        ///
        /// @brief  Provides a BrowserHost implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXBrowserHost :
            public FB::BrowserHost
        {
        public:
            ActiveXBrowserHost(IWebBrowser2 *doc, IOleClientSite* clientSite);
            virtual ~ActiveXBrowserHost(void);
            virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const override;

            virtual void *getContextID() const override;

            virtual FB::BrowserStreamPtr _createStream( const BrowserStreamRequest& req ) const;
            virtual FB::BrowserStreamPtr _createUnsolicitedStream(const BrowserStreamRequest& req) const { return FB::BrowserStreamPtr(); }

            IDispatchEx* getJSAPIWrapper(const FB::JSAPIWeakPtr& api, bool autoRelease = false);
            IDispatchWRef getIDispatchRef(IDispatch* obj);

			virtual Promise<VariantList> GetArrayValues(FB::JSObjectPtr obj);
			virtual Promise<VariantMap> GetObjectValues(FB::JSObjectPtr obj);

        public:
			IDispatchAPIPtr getPromiseObject();
            FB::DOM::DocumentPtr getDOMDocument() override;
            FB::DOM::WindowPtr getDOMWindow() override;
            FB::DOM::ElementPtr getDOMElement() override;
            void Navigate(std::string url, std::string target) override;
            
            bool hasHTMLWindow() { return !!m_htmlWin; }

            void suspend();
            void resume(IWebBrowser2 *doc, IOleClientSite* clientSite);
            
            int delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
                const FB::VariantList& args, std::string fname = "") override;
            void evaluateJavaScript(const std::string &script) override;
            void shutdown() override;

        public:
            FB::DOM::WindowPtr _createWindow(const FB::JSObjectPtr& obj) const;
            FB::DOM::DocumentPtr _createDocument(const FB::JSObjectPtr& obj) const;
            FB::DOM::ElementPtr _createElement(const FB::JSObjectPtr& obj) const;
            FB::DOM::NodePtr _createNode(const FB::JSObjectPtr& obj) const;

        protected:
            void initDOMObjects(); // This is const so that getDOMDocument/Window can be
            CComQIPtr<IOleClientSite> m_spClientSite;
            CComQIPtr<IDispatch> m_htmlDocDisp;
            CComPtr<IHTMLWindow2> m_htmlWin;
            CComPtr<IWebBrowser2> m_webBrowser;
            mutable FB::DOM::WindowPtr m_window;
            mutable FB::DOM::DocumentPtr m_document;
            std::unique_ptr<FB::WinMessageWindow> m_messageWin;

        private:
            mutable boost::shared_mutex m_xtmutex;
            mutable FB::SafeQueue<IDispatchWRef> m_deferredObjects;
            using IDispatchExRefMap = std::map<void*, FB::WeakIDispatchExRef>;
            using IDispatchRefList = std::list<IDispatchSRef>;
            mutable IDispatchExRefMap m_cachedIDispatch;
            mutable IDispatchRefList m_heldIDispatch;

        public:
            FB::variant getVariant(const VARIANT *cVar);
            void getComVariant(VARIANT *dest, const FB::variant &var);
            void deferred_release( const IDispatchWRef& obj ) const;
            void DoDeferredRelease() const;
            void ReleaseAllHeldObjects();
        };
    }
}

#endif // H_ACTIVEXBROWSERHOST

