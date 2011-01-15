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
#include "BrowserHost.h"
#include "APITypes.h"
#include "FBPointers.h"

namespace FB {
    namespace ActiveX {
        FB_FORWARD_PTR(ActiveXBrowserHost);
        FB_FORWARD_PTR(IDispatchAPI);

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
            virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const;

            virtual void *getContextID() const;

            virtual FB::BrowserStreamPtr createStream(const std::string& url, const FB::PluginEventSinkPtr& callback, 
                                                    bool cache = true, bool seekable = false, 
                                                    size_t internalBufferSize = 128 * 1024 ) const;

            void setWindow(HWND wnd);

        public:
            FB::DOM::DocumentPtr getDOMDocument();
            FB::DOM::WindowPtr getDOMWindow();
            FB::DOM::ElementPtr getDOMElement();
            void evaluateJavaScript(const std::string &script);
			void shutdown();

        public:
            FB::DOM::WindowPtr _createWindow(const FB::JSObjectPtr& obj) const;
            FB::DOM::DocumentPtr _createDocument(const FB::JSObjectPtr& obj) const;
            FB::DOM::ElementPtr _createElement(const FB::JSObjectPtr& obj) const;
            FB::DOM::NodePtr _createNode(const FB::JSObjectPtr& obj) const;

        protected:
            void initDOMObjects(); // This is const so that getDOMDocument/Window can be
            HWND m_hWnd;
        	CComQIPtr<IOleClientSite> m_spClientSite;
            CComQIPtr<IHTMLDocument2> m_htmlDoc;
            CComQIPtr<IDispatch> m_htmlDocDisp;
            CComPtr<IHTMLWindow2> m_htmlWin;
            CComPtr<IWebBrowser2> m_webBrowser;
            CComQIPtr<IDispatch> m_htmlWinDisp;
            mutable FB::DOM::WindowPtr m_window;
            mutable FB::DOM::DocumentPtr m_document;

        public:
            FB::variant getVariant(const VARIANT *cVar);
            void getComVariant(VARIANT *dest, const FB::variant &var);
        };
    }
}

#endif // H_ACTIVEXBROWSERHOST
