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
#ifndef H_FBCONTROL
#define H_FBCONTROL

#include "resource.h"       // main symbols
#include "Win/win_common.h"
#include "COM_config.h"
#include "config.h"
#include <atlctl.h>
#include "FireBreathWin_i.h"
#include "JSAPI_IDispatchEx.h"
#include "DOM/JSAPI_DOMWindow.h"
#include "Win/FactoryDefinitionsWin.h"

#include "BrowserPlugin.h"
#include "PluginCore.h"
#include "Win/PluginWindowWin.h"

#include "registrymap.hpp"

extern std::string g_dllPath;

// CFBControl
class ATL_NO_VTABLE CFBControl :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CFBControl, &CLSID_FBControl>,
    public CComControl<CFBControl>,

    public JSAPI_IDispatchEx<CFBControl, IFBControl, &DIID_IFBComEventSource>,
    public IObjectSafetyImpl<CFBControl, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/ >,

    // Required for standard events
    public IProvideClassInfo2Impl<&CLSID_FBControl, NULL, &FB_LIBID>,

    //public IPersistStreamInitImpl<CFBControl>,
    public IOleControlImpl<CFBControl>,
    public IOleObjectImpl<CFBControl>,
    public IOleInPlaceActiveObjectImpl<CFBControl>,
    public IQuickActivateImpl<CFBControl>,
    public IViewObjectExImpl<CFBControl>,
    public IOleInPlaceObjectWindowlessImpl<CFBControl>,

    public IObjectWithSiteImpl<CFBControl>,

    // Provides methods for getting <params>
    public IPersistPropertyBag,

    public FB::BrowserPlugin
{
public:
    FB::PluginWindowWin *pluginWin;
    CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> m_htmlDoc;
    CComQIPtr<IDispatch, &IID_IDispatch> m_htmlDocIDisp;

    ActiveXBrowserHostPtr m_host;

    // The methods in this class are positioned in this file in the
    // rough order that they will be called in.
    CFBControl() : pluginWin(NULL)
    {
        FB::PluginCore::setPlatform("Windows", "IE");
        setFSPath(g_dllPath);
        m_bWindowOnly = TRUE;
    }

    // Note that the window has not been created yet; this is where we get
    // access to the DOM Document and Window
    STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
    {
        HRESULT hr = IOleObjectImpl<CFBControl>::SetClientSite (pClientSite);
        if (!pClientSite)
            return hr;

        CComPtr<IOleContainer> container;

        if (m_spClientSite.p)
            m_spClientSite->GetContainer(&container);
        if (container.p) {
            m_htmlDoc = container;
            m_propNotify = m_spClientSite;
            m_htmlDocIDisp = container;
        }

        m_host = ActiveXBrowserHostPtr(new ActiveXBrowserHost(m_htmlDoc));
        pluginMain->SetHost(as_BrowserHost(m_host));
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
        m_host->setWindow(m_hWnd);
        pluginWin = _createPluginWindow(m_hWnd);
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
        delete pluginWin; pluginWin = NULL;
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

BEGIN_REGMAP(CFBControl)
    REGMAP_ENTRY("PROGID", ACTIVEX_PROGID)
    REGMAP_ENTRY("VERSION", VERSION_NUM)
    REGMAP_ENTRY("DESCRIPTION", FBControl_DESC)
    REGMAP_ENTRY("MOZILLA_PLUGINID", MOZILLA_PLUGINID)
    REGMAP_UUID("CLSID", CLSID_FBControl)
    REGMAP_UUID("LIBID", FB_LIBID)
    REGMAP_ENTRY("THREADING", "Single")
    REGMAP_ENTRY("MIMETYPE", FBSTRING_MIMEType)
    REGMAP_ENTRY("EXTENSION", FBSTRING_FileExtents)
END_REGMAP()

DECLARE_NOT_AGGREGATABLE(CFBControl)

BEGIN_COM_MAP(CFBControl)
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

OBJECT_ENTRY_AUTO(__uuidof(FBControl), CFBControl)

#endif
