/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 6, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_COMJAVASCRIPTOBJECT
#define H_COMJAVASCRIPTOBJECT

#include "FireBreathWin_i.h"
#include "JSAPI_IDispatchEx.h"

class ActiveXBrowserHost;

class ATL_NO_VTABLE COMJavascriptObject :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<COMJavascriptObject, &CLSID_FBComJavascriptObject>,

    public JSAPI_IDispatchEx<COMJavascriptObject, IFBComJavascriptObject, &DIID_IFBComEventSource>,
    public IObjectSafetyImpl<COMJavascriptObject, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/ >,

    // Required for standard events
    public IProvideClassInfo2Impl<&CLSID_FBComJavascriptObject, NULL, &FB_LIBID>
{
public:
    COMJavascriptObject(void)
    {
    }
    ~COMJavascriptObject(void)
    {
    }

    static IDispatchEx *NewObject(ActiveXBrowserHost *host, FB::JSAPI *api)
    {
        CComObject<COMJavascriptObject> *obj;
        HRESULT hr = CComObject<COMJavascriptObject>::CreateInstance(&obj);
        
        obj->setAPI(api, host);
        IDispatchEx *retval;
        hr = obj->QueryInterface(IID_IDispatchEx, (void **)&retval);

        return retval;
    }

DECLARE_NOT_AGGREGATABLE(COMJavascriptObject)

BEGIN_COM_MAP(COMJavascriptObject)
    COM_INTERFACE_ENTRY(IFBComJavascriptObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IDispatchEx)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IConnectionPoint)
    COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }
};

#endif
