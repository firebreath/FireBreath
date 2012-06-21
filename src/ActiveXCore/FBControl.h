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
#include "win_targetver.h"
#include <atlctl.h>
#include <ShlGuid.h>
#include <boost/cast.hpp>
#include <boost/scoped_array.hpp>
#include "DOM/Window.h"
#include "FactoryBase.h"
#include "logging.h"
#include "JSAPI_IDispatchEx.h"
#include "PluginInfo.h"
#include "ShareableReference.h"

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
            boost::scoped_ptr<FB::PluginWindow> pluginWin;
            CComQIPtr<IServiceProvider> m_serviceProvider;
            CComQIPtr<IWebBrowser2> m_webBrowser;
            const std::string m_mimetype;

            DWORD m_dwCurrentSafety;

            ActiveXBrowserHostPtr m_host;

        protected:

        public:
            // The methods in this class are positioned in this file in the
            // rough order that they will be called in.
            CFBControl() : JSAPI_IDispatchEx<CFBControlX, ICurObjInterface, piid>(pMT), FB::BrowserPlugin(pMT),
                m_mimetype(pMT)
            {
                setFSPath(g_dllPath);
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

            // This method will only be called if we are instantiated without a window
            STDMETHOD(SetSite)(IUnknown *pUnkSite);

            STDMETHOD(SetObjectRects)(LPCRECT prcPos, LPCRECT prcClip);
            STDMETHOD(InPlaceActivate)(LONG iVerb, const RECT* prcPosRect);
    
            // Called when the control is deactivated when it's time to shut down
            STDMETHOD(InPlaceDeactivate)(void);

            STDMETHOD(Close)(DWORD dwSaveOption) {
                shutdown();
                return IOleObjectImpl<CFBControlX>::Close(dwSaveOption);
            }

            /* IPersistPropertyBag calls */
            // This will be called once when the browser initializes the property bag (PARAM tags) 
            // Often (always?) this is only called if there are no items in the property bag
            STDMETHOD(InitNew)();

            // When this is called, we load any <param> tag values there are
            STDMETHOD(Load)(IPropertyBag *pPropBag, IErrorLog *pErrorLog);

            // This is called on shutdown
            void shutdown();

            // This is part of the event system
            STDMETHOD(GetClassID)(CLSID *pClassID);

            // This is a required method for implementing IPersistPropertyBag, but it shouldn't
            // ever get called
            STDMETHOD(Save)(IPropertyBag *pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties);

            virtual HRESULT OnDraw(ATL_DRAWINFO& di);

            void invalidateWindow( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
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
        HRESULT FB::ActiveX::CFBControl<pFbCLSID, pMT, ICurObjInterface, piid, plibid>::OnDraw( ATL_DRAWINFO& di )
        {
            if (pluginWin && m_bWndLess && FB::pluginGuiEnabled()) {
                LRESULT lRes(0);
                PluginWindowlessWin* win = static_cast<PluginWindowlessWin*>(pluginWin.get());
                FB::Rect bounds = { di.prcBounds->top, di.prcBounds->left, di.prcBounds->bottom, di.prcBounds->right };
                win->HandleDraw(di.hdcDraw, bounds);
            }
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void FB::ActiveX::CFBControl<pFbCLSID, pMT, ICurObjInterface, piid, plibid>::invalidateWindow( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
        {
            if (!m_host->isMainThread() && m_spInPlaceSite) {
                boost::shared_ptr<FB::ShareableReference<CFBControlX> > ref(boost::make_shared<FB::ShareableReference<CFBControlX> >(this));
                m_host->ScheduleOnMainThread(ref, boost::bind(&CFBControlX::invalidateWindow, this, left, top, right, bottom));
            } else {
                if (m_spInPlaceSite)
                    m_spInPlaceSite->InvalidateRect(NULL, TRUE);
            }
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        DWORD FB::ActiveX::CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::getSupportedObjectSafety()
        {
            return INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/;
        }


        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::SetSite(IUnknown *pUnkSite)
        {
            HRESULT hr = IObjectWithSiteImpl<CFBControl<pFbCLSID,pMT,ICurObjInterface,piid,plibid> >::SetSite(pUnkSite);
            if (!pUnkSite || !pluginMain) {
                shutdown();
                return hr;
            }
            m_serviceProvider = pUnkSite;
            if (!m_serviceProvider)
                return E_FAIL;
            m_serviceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void**>(&m_webBrowser));
            m_serviceProvider.Release();

            if (m_webBrowser) {
                m_propNotify = m_spClientSite;
            }

            // There will be no window this time!
            clientSiteSet();
            pluginMain->setScriptingOnly(true);
            setReady();
            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::SetClientSite( IOleClientSite *pClientSite )
        {
            HRESULT hr = IOleObjectImpl<CFBControlX>::SetClientSite (pClientSite);
            if (!pClientSite || !pluginMain) {
                shutdown();
                return hr;
            }

            m_serviceProvider = pClientSite;
            if (!m_serviceProvider)
                return E_FAIL;
            m_serviceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void**>(&m_webBrowser));
            m_serviceProvider.Release();

            if (m_webBrowser) {
                m_propNotify = m_spClientSite;
            }

            clientSiteSet();

            return S_OK;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::SetObjectRects(LPCRECT prcPos, LPCRECT prcClip)
        {
            HRESULT hr = IOleInPlaceObjectWindowlessImpl<CFBControlX>::SetObjectRects(prcPos, prcClip);

            if (m_bWndLess && pluginWin) {
                FB::PluginWindowlessWin* ptr(static_cast<FB::PluginWindowlessWin*>(pluginWin.get()));
                ptr->setWindowClipping(prcClip->top, prcClip->left, prcClip->bottom, prcClip->right);
                ptr->setWindowPosition(prcPos->left, prcPos->top, prcPos->right-prcPos->left, prcPos->bottom-prcPos->top);
            }
            return hr;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::InPlaceActivate( LONG iVerb, const RECT* prcPosRect)
        {
            m_bWindowOnly = (FB::pluginGuiEnabled() && !pluginMain->isWindowless());

            HRESULT hr = CComControl<CFBControlX>::InPlaceActivate(iVerb, prcPosRect);

            if (m_host)
                m_host->resume(m_webBrowser, m_spClientSite);
            
            if (hr != S_OK)
                return hr;

            if (pluginWin || !FB::pluginGuiEnabled() || !pluginMain) {
                // window already created or gui disabled
                return hr;
            }
            if (m_bWndLess) {
                pluginWin.swap(boost::scoped_ptr<PluginWindow>(getFactoryInstance()->createPluginWindowless(FB::WindowContextWindowless(NULL))));
                FB::PluginWindowlessWin* ptr(static_cast<FB::PluginWindowlessWin*>(pluginWin.get()));
                ptr->setInvalidateWindowFunc(boost::bind(&CFBControlX::invalidateWindow, this, _1, _2, _3, _4));
                if (m_spInPlaceSite) {
                    HWND hwnd = 0;
					HRESULT hr2 = m_spInPlaceSite->GetWindow(&hwnd);
                    if (SUCCEEDED(hr2)) {
                        ptr->setHWND(hwnd);
                    }
                }
            } else {
                pluginWin.swap(boost::scoped_ptr<PluginWindow>(getFactoryInstance()->createPluginWindowWin(FB::WindowContextWin(m_hWnd))));
                static_cast<PluginWindowWin*>(pluginWin.get())->setCallOldWinProc(true);
            }
            pluginMain->SetWindow(pluginWin.get());

            return hr;
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        STDMETHODIMP CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::InPlaceDeactivate( void )
        {
            if (pluginMain) {
                pluginMain->ClearWindow();
                pluginWin.reset();
            }
            // We have to release all event handlers and other held objects at this point, because
            // if we don't the plugin won't shut down properly; normally it isn't an issue to do
            // so, but note that this gets called if you move the plugin around in the DOM!
            if (m_host) {
                m_host->ReleaseAllHeldObjects();
                m_connPtMap.clear();
                m_host->suspend();
            }
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
            pluginMain->SetHost(FB::ptr_cast<FB::BrowserHost>(m_host));
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::setReady()
        {
            // This is when we can consider the plugin "ready".  The window will not be around yet!
            this->setAPI(pluginMain->getRootJSAPI(), m_host);
            setReadyState(READYSTATE_COMPLETE);
            pluginMain->setReady();
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::setWindow( HWND hWnd )
        {
        }

        template <const GUID* pFbCLSID, const char* pMT, class ICurObjInterface, const IID* piid, const GUID* plibid>
        void CFBControl<pFbCLSID, pMT,ICurObjInterface,piid,plibid>::shutdown()
        {
            // the host must be shut down before the rest to prevent deadlocks on async calls
            if (m_host)
                m_host->shutdown();

            if (pluginMain) {
                pluginMain->ClearWindow(); //Already done during InPlaceDeactivate
                pluginMain->shutdown();
            }

            if (pluginWin) {
                pluginWin.reset();
            }

            m_api.reset(); // Once we release this, pluginMain releasing should free it
            pluginMain.reset(); // This should delete the plugin object
            m_propNotify.Release();
            m_webBrowser.Release();
            m_serviceProvider.Release();
            m_connPtMap.clear();
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
                // Set Focus & Capture whenever a button is pushed inside of the plugin instance.
                switch(uMsg)
                {
                case WM_LBUTTONDOWN:
                case WM_MBUTTONDOWN:
                case WM_RBUTTONDOWN:
                    if (m_bNegotiatedWnd && m_bWndLess && m_spInPlaceSite) {
                        m_spInPlaceSite->SetFocus(true);
                        m_spInPlaceSite->SetCapture(true);
                    }
                    break;
                case WM_LBUTTONUP:
                case WM_MBUTTONUP:
                case WM_RBUTTONUP:
                    if (m_bNegotiatedWnd && m_bWndLess && m_spInPlaceSite) {
                        m_spInPlaceSite->SetCapture(false);
                    }
                    break;
                case WM_MOUSEACTIVATE:
                    break;
                    //lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
                    //return TRUE;
                }

                if (bHandled)
                    return TRUE;
                else if (m_bWndLess && pluginWin && static_cast<PluginWindowlessWin*>(pluginWin.get())->HandleEvent(uMsg, wParam, lParam, lResult))
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

#endif

