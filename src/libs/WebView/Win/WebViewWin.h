/**********************************************************\
Original Author: Vijaye Raji

Created:    Jun 23, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/

#pragma once
#ifndef WebViewWin_h__
#define WebViewWin_h__

#include "win_common.h"
#include "WebView.h"

#include <Windows.h>
#include <exdisp.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <exdispid.h>

#include "SupportComImpl.h"
#include "ActiveXBrowserHost.h"
#include "COMJavascriptObject.h"
#include "ApiTypes.h"
#include "Win/PluginWindowWin.h"
#include "ActiveXFactoryDefinitions.h"
#include "PluginEvents/AttachedEvent.h"

namespace FB { namespace View {
    class WebBrowserEventHandler;

    class WebViewWin : public WebView
    {
    public:
        WebViewWin(const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost);
        virtual ~WebViewWin();

    public:
        // Page Management
        void loadUri(const FB::URI& uri);
        void loadHtml(const std::string& html);
        void closePage();

        // Accessing the page
        FB::BrowserHostPtr getPageHost() {
            return fbBrowserHostPtr;
        }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindowWin)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindowWin)
    END_PLUGIN_EVENT_MAP()

        bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindowWin *wnd);
        bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindowWin *wnd);

    private:
        HRESULT ConstructBrowser();
        IHTMLDocument2* GetDocument();
        void initPage();
        void InjectCustomObjects();
        void Resize(RECT rect);

        CComPtr<IOleObject> browserObject;
        CComPtr<IWebBrowser2> webBrowser;
        boost::scoped_ptr<OleClientSite> clientSite;
        boost::scoped_ptr<IStorage> oleStorage;
        HWND browserWindow;
        HWND hostWindow;
        FB::ActiveX::ActiveXBrowserHostPtr fbBrowserHostPtr;

    private:
        void OnDownloadComplete();
        void OnDocumentReady(const bool pageNeedsInit = true);
        void OnBeforeNavigate();
        friend class WebBrowserEventHandler;
    };

    class WebBrowserEventHandler : public DWebBrowserEvents2 {
    private:
        WebViewWin* webBrowser;

    public:
        WebBrowserEventHandler(WebViewWin* webBrowser) {
            this->webBrowser = webBrowser;
        }

        HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) {
            if (riid == DIID_DWebBrowserEvents2) {
                *ppvObject = this;
                return S_OK;
            }
            return E_NOINTERFACE;
        }
        ULONG __stdcall AddRef() { return 1; }
        ULONG __stdcall Release() { return 1; }
        HRESULT __stdcall GetTypeInfoCount(UINT *pctinfo) { return E_NOTIMPL; }
        HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) { return E_NOTIMPL; }
        HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) { return E_NOTIMPL; }
        HRESULT __stdcall Invoke(
            DISPID dispIdMember,
            REFIID riid,
            LCID lcid,
            WORD wFlags,
            DISPPARAMS *pDispParams,
            VARIANT *pVarResult,
            EXCEPINFO *pExcepInfo,
            UINT *puArgErr)
        {
            switch (dispIdMember) {
            case DISPID_DOWNLOADCOMPLETE:
                webBrowser->OnDownloadComplete();
                break;

            case DISPID_BEFORENAVIGATE2:
                webBrowser->OnBeforeNavigate();
                break;

            case DISPID_DOCUMENTCOMPLETE:
                webBrowser->OnDocumentReady();
                break;
            }

            return S_OK;
        }
    };

}}
#endif // WebViewWin_h__
