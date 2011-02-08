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

#pragma once
#ifndef H_COMJAVASCRIPTOBJECT
#define H_COMJAVASCRIPTOBJECT

#include "JSAPI_IDispatchEx.h"

namespace FB {
    namespace ActiveX {
        class ActiveXBrowserHost;

        template <const GUID* pclsid, class ICurObjInterface, const IID* piid, const GUID* plibid>
        class ATL_NO_VTABLE COMJavascriptObject :
            public CComObjectRootEx<CComMultiThreadModel>,
            public CComCoClass<COMJavascriptObject<pclsid, ICurObjInterface, piid, plibid>, pclsid>,

            public JSAPI_IDispatchEx<COMJavascriptObject<pclsid, ICurObjInterface, piid, plibid>, ICurObjInterface, piid>,
            public IObjectSafetyImpl<COMJavascriptObject<pclsid, ICurObjInterface, piid, plibid>, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA/* | INTERFACE_USES_DISPEX*/ >,

            // Required for standard events
            public IProvideClassInfo2Impl<pclsid, NULL, plibid>
        {
        public:
            typedef COMJavascriptObject<pclsid, ICurObjInterface, piid, plibid> CurObjType;
            COMJavascriptObject(void) : JSAPI_IDispatchEx<CurObjType, ICurObjInterface, piid>("SUBOBJECT")
            {
            }
            virtual ~COMJavascriptObject(void)
            {
            }

            static IDispatchEx *NewObject(ActiveXBrowserHostPtr host, FB::JSAPIWeakPtr api)
            {
                CComObject<CurObjType> *obj;
                HRESULT hr = CComObject<CurObjType>::CreateInstance(&obj);
                
                obj->setAPI(api, host);
                IDispatchEx *retval;
                hr = obj->QueryInterface(IID_IDispatchEx, (void **)&retval);

                return retval;
            }

        DECLARE_NOT_AGGREGATABLE(CurObjType)

        BEGIN_COM_MAP(CurObjType)
            COM_INTERFACE_ENTRY_IID((*piid), ICurObjInterface)
            COM_INTERFACE_ENTRY(IDispatch)
            COM_INTERFACE_ENTRY(IDispatchEx)
            COM_INTERFACE_ENTRY(IFireBreathObject)
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
    };
};
#endif

