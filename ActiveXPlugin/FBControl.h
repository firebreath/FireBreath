/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FBControl.h : Declaration of the CFBControl
#ifndef H_FBCONTROL
#define H_FBCONTROL

#include "resource.h"       // main symbols
#include "COM_config.h"
#include "config.h"
#include <atlctl.h>
#include "FireBreathWin_i.h"
#include "BrowserPlugin.h"
#include "JSAPI_IDispatchEx.h"

#include "registrymap.hpp"

// CFBControl
class ATL_NO_VTABLE CFBControl :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFBControl, &CLSID_FBControl>,
    public CComControl<CFBControl>,

    public JSAPI_IDispatchEx<CFBControl, IFBControl, &DIID_IFBComEventSource>,
    public IObjectSafetyImpl<CFBControl, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA | INTERFACE_USES_DISPEX >,

    // Required for standard events
    public IProvideClassInfo2Impl<&CLSID_FBControl, NULL, &LIBID_FireBreathWinLib>,

    // Provides methods for getting <params>
    //public IPersistPropertyBag,

    //public IPersistStreamInitImpl<CFBControl>,
    public IObjectWithSiteImpl<CFBControl>,
    public IOleControlImpl<CFBControl>,
    public IOleObjectImpl<CFBControl>,
    public IOleInPlaceActiveObjectImpl<CFBControl>,
    public IQuickActivateImpl<CFBControl>,
    public IViewObjectExImpl<CFBControl>,
    public IOleInPlaceObjectWindowlessImpl<CFBControl>,
    public FB::BrowserPlugin
{
public:

    CFBControl()
    {
        m_bWindowOnly = TRUE;
        this->setAPI(FB::BrowserPlugin::m_api, new ActiveXBrowserHost(NULL));
    }

    void shutdown()
    {
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_host->setWindow(m_hWnd);
        return S_OK;
    }

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_INSIDEOUT |
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID_EX(IDR_FBCONTROL)

BEGIN_REGMAP(CFBControl)
    REGMAP_ENTRY("PROGID", ACTIVEX_PROGID)
    REGMAP_ENTRY("VERSION", VERSION_NUM)
    REGMAP_ENTRY("DESCRIPTION", FBControl_DESC)
    REGMAP_ENTRY("MOZILLA_PLUGINID", MOZILLA_PLUGINID)
    REGMAP_UUID("CLSID", CLSID_FBControl)
    REGMAP_UUID("LIBID", LIBID_FireBreathWinLib)
    REGMAP_ENTRY("THREADING", "Single")
    REGMAP_ENTRY("MIMETYPE", FBSTRING_MIMEType)
    REGMAP_ENTRY("EXTENSION", FBSTRING_FileExtents)
END_REGMAP()

DECLARE_NOT_AGGREGATABLE(CFBControl)

BEGIN_COM_MAP(CFBControl)
    COM_INTERFACE_ENTRY(IFBControl)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IDispatchEx)
    COM_INTERFACE_ENTRY(IObjectSafety)
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
    //COM_INTERFACE_ENTRY(IPersistStreamInit)
    //COM_INTERFACE_ENTRY(IPersistPropertyBag)
    //COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
    COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult,
                              DWORD dwMsgMapID = 0);

// IViewObjectEx
    DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IFBControl
    STDMETHOD(TranslateAccelerator)(LPMSG pMsg)
    {
        //CComPtr<IOleInPlaceActiveObject> spIOleInPlaceActiveObject;

        //HRESULT hr = m_spBrowser->QueryInterface(&spIOleInPlaceActiveObject);
        //if (SUCCEEDED(hr))
        //    hr = spIOleInPlaceActiveObject->TranslateAccelerator(pMsg);
        //if (hr != S_OK)
        //    hr = IOleInPlaceActiveObjectImpl<CFBControl>::TranslateAccelerator(pMsg);

        return S_OK; //hr;
    }

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }
};

OBJECT_ENTRY_AUTO(__uuidof(FBControl), CFBControl)

#endif