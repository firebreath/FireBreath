/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FBControl.h : Declaration of the CFBControl
#pragma once
#ifndef H_FBCONTROL
#define H_FBCONTROL

#include "win_common.h"
#include <atlctl.h>
#include <ShlGuid.h>
#include "DOM/Window.h"
#include "FactoryBase.h"
#include "logging.h"
#include "Win/WinMessageWindow.h"
#include "JSAPI_IDispatchEx.h"
#include "PluginInfo.h"

#include "BrowserPlugin.h"
#include "PluginCore.h"
#include "Win/WindowContextWin.h"

#include "registrymap.hpp"

extern std::string g_dllPath;

// CFBControl
namespace FB {
    namespace ActiveX {
        class ActiveXBrowserHost;
        typedef boost::shared_ptr<ActiveXBrowserHost> ActiveXBrowserHostPtr;
        void flagStaticInitialized(bool init = true);
        bool isStaticInitialized();

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        class ATL_NO_VTABLE CFBControl :
            public CComObjectRootEx<CComMultiThreadModel>,
            public CComCoClass<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid>, pFbCLSID >,
            public CComControl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,

            public JSAPI_IDispatchEx< CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid>, ICurObjInterface, piid>,

            // Required for standard events
            public IProvideClassInfo2Impl<pFbCLSID, NULL, plibid>,
            public IObjectSafety,

            //public IPersistStreamInitImpl<CFBControl<pFbCLSID,pMT> >,
            public IOleControlImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,
            public IOleObjectImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,
            public IOleInPlaceActiveObjectImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,
            public IQuickActivateImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,
            public IViewObjectExImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,
            public IOleInPlaceObjectWindowlessImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,

            public IObjectWithSiteImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >,

            // Provides methods for getting <params>
            public IPersistPropertyBag,

            public FB::BrowserPlugin
        {
        public:
            typedef CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> CFBControlX;

        protected:
            FB::PluginWindowWin *pluginWin;
            CComQIPtr<IServiceProvider> m_serviceProvider;
            CComQIPtr<IWebBrowser2> m_webBrowser;
            const std::string m_mimetype;

            DWORD m_dwCurrentSafety;

            ActiveXBrowserHostPtr m_host;

        protected:
            boost::scoped_ptr<FB::WinMessageWindow> m_messageWin;

        public:
            // The methods in this class are positioned in this file in the
            // rough order that they will be called in.
            CFBControl() : JSAPI_IDispatchEx<CFBControlX, ICurObjInterface, piid>(pMT), FB::BrowserPlugin(pMT),
                pluginWin(NULL), m_mimetype(pMT)
            {
                FB::PluginCore::setPlatform("Windows", "IE");
                setFSPath(g_dllPath);
                if (FB::pluginGuiEnabled())
                    m_bWindowOnly = TRUE;
                else
                    m_bWindowOnly = FALSE;
            }

            ~CFBControl()
            {
            }

            virtual FB::VariantMap getProperties(const CComQIPtr<IPropertyBag2>& pBag);

            virtual void setReady();
            virtual void setWindow(HWND);
            virtual void clientSiteSet();
            virtual DWORD getSupportedObjectSafety();

            STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions,
                DWORD *pdwEnabledOptions);

            STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask,
                DWORD dwEnabledOptions);

            // Note that the window has not been created yet; this is where we get
            // access to the DOM Document and Window
            STDMETHOD(SetClientSite)(IOleClientSite *pClientSite);

            // Called when the control is deactivated when it's time to shut down
            STDMETHOD(InPlaceDeactivate)(void);

            /* IPersistPropertyBag calls */
            // This will be called once when the browser initializes the property bag (PARAM tags) 
            // Often (always?) this is only called if there are no items in the property bag
            STDMETHOD(InitNew)();

            // When this is called, we load any <param> tag values there are
            STDMETHOD(Load)(IPropertyBag *pPropBag, IErrorLog *pErrorLog);

            // Now the window has been created and we're going to call setReady on the PluginCore object
            LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

            // This is called on shutdown
            void shutdown();

            // This is part of the event system
            STDMETHOD(GetClassID)(CLSID *pClassID);

            // This is a required method for implementing IPersistPropertyBag, but it shouldn't
            // ever get called
            STDMETHOD(Save)(IPropertyBag *pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties);

        public:
            DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
            OLEMISC_CANTLINKINSIDE |
                OLEMISC_INSIDEOUT |
                OLEMISC_ACTIVATEWHENVISIBLE |
                OLEMISC_SETCLIENTSITEFIRST
                )

                DECLARE_REGISTRY_RESOURCEID_EX(IDR_FBCONTROL)

            BEGIN_REGMAP(CFBControlX)
                REGMAP_UUID("LIBID", (*plibid))
                //REGMAP_ENTRY("THREADING", "Free")
                REGMAP_ENTRY("THREADING", "Apartment")
                //REGMAP_ENTRY("THREADING", "Single")
                END_REGMAP()

                DECLARE_NOT_AGGREGATABLE(CFBControlX)

            BEGIN_COM_MAP(CFBControlX)
                COM_INTERFACE_ENTRY_IID((*piid), ICurObjInterface)
                COM_INTERFACE_ENTRY(IDispatch)
                COM_INTERFACE_ENTRY(IDispatchEx)
                COM_INTERFACE_ENTRY(IFireBreathObject)
                COM_INTERFACE_ENTRY(IViewObjectEx)
                COM_INTERFACE_ENTRY(IViewObject2)
                COM_INTERFACE_ENTRY(IViewObject)
                COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
                COM_INTERFACE_ENTRY(IOleInPlaceObject)
                COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
                COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
                COM_INTERFACE_ENTRY(IOleControl)
                COM_INTERFACE_ENTRY(IOleObject)
                COM_INTERFACE_ENTRY(IConnectionPointContainer)
                COM_INTERFACE_ENTRY(IConnectionPoint)
                COM_INTERFACE_ENTRY(IQuickActivate)
                COM_INTERFACE_ENTRY(IObjectWithSite)
                COM_INTERFACE_ENTRY(IObjectSafety)

                COM_INTERFACE_ENTRY(IPersistPropertyBag)
                COM_INTERFACE_ENTRY(IProvideClassInfo)
                COM_INTERFACE_ENTRY(IProvideClassInfo2)
            END_COM_MAP()

            BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult,
                DWORD dwMsgMapID = 0);

            // IViewObjectEx
            DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

            // IFBControl
            DECLARE_PROTECT_FINAL_CONSTRUCT()

            HRESULT FinalConstruct()
            {
                return S_OK;
            }

            void FinalRelease()
            {
            }
        };

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        DWORD FB::ActiveX::CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::getSupportedObjectSafety()
        {
            return INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::SetClientSite( IOleClientSite *pClientSite )
        {
            HRESULT hr = IOleObjectImpl<CFBControlX>::SetClientSite (pClientSite);
            if (!pClientSite) {
                m_webBrowser.Release();
                m_serviceProvider.Release();
                if (m_host)
                    m_host->shutdown();
                m_host.reset();
                return hr;
            }

            m_serviceProvider = pClientSite;
            if (!m_serviceProvider)
                return E_FAIL;
            m_serviceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void**>(&m_webBrowser));

            if (m_webBrowser) {
                m_propNotify = m_spClientSite;
            }

            m_messageWin.swap(boost::scoped_ptr<FB::WinMessageWindow>(new FB::WinMessageWindow()));

            clientSiteSet();

            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::InPlaceDeactivate( void )
        {
            shutdown();
            HRESULT hr = IOleInPlaceObjectWindowlessImpl<CFBControlX>::InPlaceDeactivate();
            return hr;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::InitNew()
        {
            pluginMain->setParams(FB::VariantMap());
            setReady();
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        FB::VariantMap CFBControl<pFbCLSID, pMT, ICurObjInterface, piid, plibid>::getProperties( const CComQIPtr<IPropertyBag2>& pBag )
        {
            ULONG pCount(0), pGot(0);
            HRESULT hr = pBag->CountProperties(&pCount);
            FB::VariantMap paramMap;
            boost::scoped_array<PROPBAG2> pArr(new PROPBAG2[pCount]);
            hr = pBag->GetPropertyInfo(0, pCount, pArr.get(), &pGot);

            boost::scoped_array<HRESULT> results(new HRESULT[pCount]);
            boost::scoped_array<CComVariant> vals(new CComVariant[pCount]);
            hr = pBag->Read(pGot, pArr.get(), NULL, vals.get(), results.get());
            if (SUCCEEDED(hr)) {
                for(ULONG i = 0; i < pGot; ++i) {
                    HRESULT curRes = results[i];
                    if (SUCCEEDED(curRes)) {
                        PROPBAG2* curInfo = &pArr[i];
                        VARIANT* curVal = &vals[i];
                        std::string name(FB::wstring_to_utf8(curInfo->pstrName));
                        FB::variant varval(m_host->getVariant(curVal));
                        paramMap[name] = varval;

                        // Clean up
                        CoTaskMemFree(curInfo->pstrName);
                    }
                }
            }
            return paramMap;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::Load( IPropertyBag *pPropBag, IErrorLog *pErrorLog )
        {
            pluginMain->setParams(getProperties(CComQIPtr<IPropertyBag2>(pPropBag)));

            setReady();
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::clientSiteSet()
        {
            m_host = ActiveXBrowserHostPtr(new ActiveXBrowserHost(m_webBrowser, m_spClientSite));
            m_host->setWindow(m_messageWin->getHWND());
            pluginMain->SetHost(FB::ptr_cast<FB::BrowserHost>(m_host));
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::setReady()
        {
            // This is when we can consider the plugin "ready".  The window may or may not (likely not)
            // be around yet!
            this->setAPI(pluginMain->getRootJSAPI(), m_host);
            setReadyState(READYSTATE_COMPLETE);
            pluginMain->setReady();
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        LRESULT CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::OnCreate( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
        {
            setWindow(m_hWnd);
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::setWindow( HWND hWnd )
        {
            pluginWin = getFactoryInstance()->createPluginWindowWin(FB::WindowContextWin(m_hWnd));
            pluginWin->setCallOldWinProc(true);
            pluginMain->SetWindow(pluginWin);
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::shutdown()
        {
            if (pluginMain)
                pluginMain->ClearWindow();
            if (pluginWin) {
                delete pluginWin; pluginWin = NULL;
            }
            m_api.reset(); // Once we release this, pluginMain releasing should free it
            pluginMain.reset(); // This should delete the plugin object
            m_propNotify.Release();
            m_webBrowser.Release();
            m_serviceProvider.Release();
            m_connPtMap.clear();
            if (m_host)
                m_host->shutdown();
            m_host.reset();
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::GetClassID( CLSID *pClassID )
        {
            if (pClassID == NULL)
                return E_POINTER;
            *pClassID = GetObjectCLSID();
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::Save( IPropertyBag *pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties )
        {
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
        {
            if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
                return E_POINTER;

            HRESULT hr;
            CComPtr<IUnknown> pUnk;
            // Check if we support this interface
            hr = GetUnknown()->QueryInterface(riid, (void**)&pUnk);
            if (SUCCEEDED(hr)) {
                *pdwSupportedOptions = getSupportedObjectSafety();
                *pdwEnabledOptions   = m_dwCurrentSafety;
            } else {
                // We don't support this interface
                *pdwSupportedOptions = 0;
                *pdwEnabledOptions   = 0;
            }
            return hr;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
        {
            CComPtr<IUnknown> pUnk;
            // Check if we support the interface and return E_NOINTEFACE if we don't
            if (FAILED(GetUnknown()->QueryInterface(riid, (void**)&pUnk)))
                return E_NOINTERFACE;

            // If we are asked to set options we don't support then fail
            if (dwOptionSetMask & ~getSupportedObjectSafety())
                return E_FAIL;

            m_dwCurrentSafety = (m_dwCurrentSafety & ~dwOptionSetMask) | (dwOptionSetMask & dwEnabledOptions);
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        BOOL CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
        {
            BOOL bHandled = FALSE;
            switch(dwMsgMapID)
            {
            case 0: {
                // WM_CREATE is the only message we handle here
                switch(uMsg)
                {
                case WM_CREATE:
                    lResult = OnCreate(uMsg, wParam, lParam, bHandled);
                    if(bHandled)
                        return TRUE;
                    break;

                case WM_MOUSEACTIVATE:
                    lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
                    return TRUE;

                }

                if (bHandled)
                    return TRUE;
                else if(CComControl<CFBControlX>::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
                    return TRUE;
                else if(DefaultReflectionHandler(hWnd, uMsg, wParam, lParam, lResult))
                    return TRUE;
                    } break;

            }
            return FALSE;
        }

    }
}
// This includes an auto-generated file that sets up the plugin with mimetypes
//#include "global/axplugin_defs.h"

#endif
