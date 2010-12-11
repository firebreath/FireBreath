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

#include "resource.h"       // main symbols
#include "Win/win_common.h"
#include "COM_config.h"
#include "config.h"
#include <atlctl.h>
#include <ShlGuid.h>
#include "FireBreathWin_i.h"
#include "JSAPI_IDispatchEx.h"
#include "DOM/Window.h"
#include "FactoryBase.h"
#include "logging.h"

#include "BrowserPlugin.h"
#include "PluginCore.h"
#include "Win/PluginWindowWin.h"
#include "Win/WindowContextWin.h"

#include "registrymap.hpp"

extern std::string g_dllPath;

// CFBControl
template <const GUID* pFbCLSID, const char* pMT>
class ATL_NO_VTABLE CFBControl :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFBControl<pFbCLSID,pMT>, pFbCLSID >,
    public CComControl<CFBControl<pFbCLSID,pMT> >,

    public JSAPI_IDispatchEx<CFBControl<pFbCLSID,pMT>, IFBControl, &DIID_IFBComEventSource>,
    public IObjectSafetyImpl<CFBControl<pFbCLSID,pMT>, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/ >,

    // Required for standard events
    public IProvideClassInfo2Impl<pFbCLSID, NULL, &FB_LIBID>,

    //public IPersistStreamInitImpl<CFBControl<pFbCLSID,pMT> >,
    public IOleControlImpl<CFBControl<pFbCLSID,pMT> >,
    public IOleObjectImpl<CFBControl<pFbCLSID,pMT> >,
    public IOleInPlaceActiveObjectImpl<CFBControl<pFbCLSID,pMT> >,
    public IQuickActivateImpl<CFBControl<pFbCLSID,pMT> >,
    public IViewObjectExImpl<CFBControl<pFbCLSID,pMT> >,
    public IOleInPlaceObjectWindowlessImpl<CFBControl<pFbCLSID,pMT> >,

    public IObjectWithSiteImpl<CFBControl<pFbCLSID,pMT> >,

    // Provides methods for getting <params>
    public IPersistPropertyBag,

    public FB::BrowserPlugin
{
public:
    typedef CFBControl<pFbCLSID, pMT> CFBControlX;
    HWND m_messageWin;

    FB::PluginWindowWin *pluginWin;
    CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> m_htmlDoc;
    CComQIPtr<IDispatch, &IID_IDispatch> m_htmlDocIDisp;
    CComQIPtr<IServiceProvider> m_serviceProvider;
    CComQIPtr<IWebBrowser2> m_webBrowser;

    ActiveXBrowserHostPtr m_host;

private:
    static int _REFCOUNT; // This must only be accessed from the main thread

public:
    // The methods in this class are positioned in this file in the
    // rough order that they will be called in.
    CFBControl() : pluginWin(NULL), m_messageWin(NULL)
    {
        if (_REFCOUNT++ == 0) {
            FB::Log::initLogging();
        }
        FB::PluginCore::setPlatform("Windows", "IE");
        setFSPath(g_dllPath);
#if FB_GUI_DISABLED == 1
        m_bWindowOnly = FALSE;
#else
        m_bWindowOnly = TRUE;
#endif
    }

    ~CFBControl()
    {
        if (--_REFCOUNT == 0) {
            FB::Log::stopLogging();
        }
        if (m_messageWin)
            ::DestroyWindow(m_messageWin);
    }

    // Note that the window has not been created yet; this is where we get
    // access to the DOM Document and Window
    STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
    {
        HRESULT hr = IOleObjectImpl<CFBControlX>::SetClientSite (pClientSite);
        if (!pClientSite)
            return hr;

        m_serviceProvider = pClientSite;
        if (!m_serviceProvider)
            return E_FAIL;
        m_serviceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void**>(&m_webBrowser));

        if (m_webBrowser.p) {
            m_htmlDoc = m_webBrowser;
            m_propNotify = m_spClientSite;
            m_htmlDocIDisp = m_webBrowser;
        }

        m_host = ActiveXBrowserHostPtr(new ActiveXBrowserHost(m_webBrowser));
        m_messageWin = FB::PluginWindowWin::createMessageWindow();
        m_host->setWindow(m_messageWin);
        pluginMain->SetHost(FB::ptr_cast<FB::BrowserHost>(m_host));
        this->setAPI(pluginMain->getRootJSAPI(), m_host);
        //InPlaceActivate(OLEIVERB_UIACTIVATE);
        //this->FireOnChanged(DISPID_READYSTATE);

        return S_OK;
    }

    /* IPersistPropertyBag calls */
    // This will be called once when the browser initializes the property bag (PARAM tags) 
    // Often (always?) this is only called if there are no items in the property bag
    STDMETHOD(InitNew)()
    {
        return S_OK;
    }
    
    // When this is called, we load any <param> tag values there are
    STDMETHOD(Load)(IPropertyBag *pPropBag, IErrorLog *pErrorLog)
    {
        FB::VariantMap paramMap;
        FB::StringSet *paramList = this->pluginMain->getSupportedParams();
        for (FB::StringSet::iterator it = paramList->begin(); it != paramList->end(); it++) {
            CComVariant val;
            pPropBag->Read(FB::utf8_to_wstring(*it).c_str(), &val, pErrorLog);
            if (val.vt) {
                FB::variant varval = m_host->getVariant(&val);
                paramMap[it->c_str()] = varval;
            }
        }
        pluginMain->setParams(paramMap);
        return S_OK;
    }

    // Now the window has been created and we're going to call setReady on the PluginCore object
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        pluginWin = getFactoryInstance()->createPluginWindowWin(FB::WindowContextWin(m_hWnd));
        pluginWin->setCallOldWinProc(true);
        pluginMain->SetWindow(pluginWin);

        // This is when we can consider the plugin "ready".  It is the last of the startup lifecycle to occur.
        pluginMain->setReady();
        setReadyState(READYSTATE_COMPLETE);
        return S_OK;
    }

    // This is called on shutdown
    void shutdown()
    {
        pluginMain->ClearWindow();
        delete pluginWin; pluginWin = NULL;
        pluginMain.reset(); // This should delete the plugin object
        m_api.reset();
    }

    // This is part of the event system
    STDMETHOD(GetClassID)(CLSID *pClassID)
    {
        if (pClassID == NULL)
            return E_POINTER;
        *pClassID = GetObjectCLSID();
        return S_OK;
    }

    // This is a required method for implementing IPersistPropertyBag, but it shouldn't
    // ever get called
    STDMETHOD(Save)(IPropertyBag *pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties)
    {
        return S_OK;
    }

public:
DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_INSIDEOUT |
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID_EX(IDR_FBCONTROL)

BEGIN_REGMAP(CFBControlX)
    REGMAP_ENTRY("PROGID", ACTIVEX_PROGID)
    REGMAP_ENTRY("VERSION", VERSION_NUM)
    REGMAP_ENTRY("DESCRIPTION", FBControl_DESC)
    REGMAP_ENTRY("MOZILLA_PLUGINID", MOZILLA_PLUGINID)
    REGMAP_UUID("CLSID", *pFbCLSID)
    REGMAP_UUID("LIBID", FB_LIBID)
    REGMAP_ENTRY("THREADING", "Multi")
    REGMAP_ENTRY("MIMETYPE", FBSTRING_MIMEType)
    REGMAP_ENTRY("EXTENSION", FBSTRING_FileExtents)
END_REGMAP()

DECLARE_NOT_AGGREGATABLE(CFBControlX)

BEGIN_COM_MAP(CFBControlX)
    COM_INTERFACE_ENTRY(IFBControl)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IDispatchEx)
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
    //COM_INTERFACE_ENTRY(IQuickActivate)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY(IObjectSafety)

    //COM_INTERFACE_ENTRY(IPersistStreamInit)
    COM_INTERFACE_ENTRY(IPersistPropertyBag)
    //COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
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

template <const GUID* pFbCLSID, const char* pMT>
int CFBControl<pFbCLSID, pMT>::_REFCOUNT(0); // This must only be accessed from the main thread

template <const GUID* pFbCLSID, const char* pMT>
BOOL CFBControl<pFbCLSID, pMT>::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
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

        case WM_DESTROY:
            shutdown();
            break;
        }

        if (bHandled)
            return TRUE;
        else if(CComControl<CFBControl<pFbCLSID, pMT> >::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
            return TRUE;
        else if(DefaultReflectionHandler(hWnd, uMsg, wParam, lParam, lResult))
            return TRUE;
        } break;
    
    }
    return FALSE;
}

class FireBreathClassFactory : public CComClassFactory
{
	// IClassFactory
	STDMETHOD(CreateInstance)(
		LPUNKNOWN pUnkOuter, 
		REFIID riid, 
		void** ppvObj)
	{
        return CComClassFactory::CreateInstance(pUnkOuter, riid, ppvObj);
		//ATLASSUME(m_pfnCreateInstance != NULL);
		//HRESULT hRes = E_POINTER;
		//if (ppvObj != NULL)
		//{
		//	*ppvObj = NULL;
		//	// can't ask for anything other than IUnknown when aggregating

		//	if ((pUnkOuter != NULL) && !InlineIsEqualUnknown(riid))
		//	{
		//		ATLTRACE(atlTraceCOM, 0, _T("CComClassFactory: asked for non IUnknown interface while creating an aggregated object"));
		//		hRes = CLASS_E_NOAGGREGATION;
		//	}
		//	else
		//		hRes = m_pfnCreateInstance(pUnkOuter, riid, ppvObj);
		//}
		//return hRes;
	}
};

char FBCONTROL1_MIMETYPE[];
char FBCONTROL2_MIMETYPE[];

typedef CFBControl<&CLSID_FBControl, FBCONTROL1_MIMETYPE> CFBControl1;
typedef CFBControl<&CLSID_FBControl2, FBCONTROL2_MIMETYPE> CFBControl2;

OBJECT_ENTRY_AUTO(__uuidof(FBControl), CFBControl1);
OBJECT_ENTRY_AUTO(__uuidof(FBControl2), CFBControl2);
//__declspec(selectany) ATL::_ATL_OBJMAP_ENTRY __objMap_CFBControl = {
//    &__uuidof(FBControl),
//    FBControl::UpdateRegistry,
//    FBControl::_ClassFactoryCreatorFBControl::CreateInstance,
//    FBControl::_CreatorFBControl::CreateInstance,
//    NULL,
//    0,
//    FBControl::GetObjectDescription,
//    FBControl::GetCategoryMap,
//    FBControl::ObjectMain
//};
//extern "C" __declspec(allocate("ATL$__m")) __declspec(selectany) ATL::_ATL_OBJMAP_ENTRY* const __pobjMap_CFBControl = &__objMap_CFBControl;
//__pragma(comment(linker, "/include:___pobjMap_" CFBControl));

#endif
