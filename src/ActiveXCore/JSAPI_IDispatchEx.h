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
#ifndef H_JSAPI_IDISPATCHEX
#define H_JSAPI_IDISPATCHEX

#include "ActiveXBrowserHost.h"
#include "APITypes.h"
#include "JSAPI.h"
#include "TypeIDMap.h"
#include "utf8_tools.h"
#include "JSFunction.h"
#include "axutil.h"

#include "IDispatchAPI.h"
#include "dispex.h"
#include <map>
#include "logging.h"
#include <mshtmdid.h>
#include "Deferred.h"
#include "ComPromise.h"

MIDL_INTERFACE("a7f89004-620a-56e0-aeea-ec0e8da18fb3")
IFireBreathObject : public IUnknown
{
};
#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
// Global interface for all firebreath plugins
MIDL_DEFINE_GUID(IID, IID_IFireBreathObject,0xa7f89004,0x620a,0x56e0,0xae,0xea,0xec,0x0e,0x8d,0xa1,0x8f,0xb3);
#undef MIDL_DEFINE_GUID

namespace FB { namespace ActiveX {
    class JSAPI_IDispatchExBase : public IFireBreathObject
    {
    public:
    public:
        JSAPI_IDispatchExBase() 
        { }
        void setAPI(FB::JSAPIWeakPtr api, const ActiveXBrowserHostPtr& host)
        {
            m_api = api;
            m_host = host;
        }

        FB::JSAPIPtr getAPI()
        {
            FB::JSAPIPtr api(m_api.lock());
            if (!api)
                throw FB::bad_cast_ext("Invalid object");
            return api;
        }
        ActiveXBrowserHostPtr getHost() const {
            return m_host.lock();
        }

        friend class IDisp_AttachEvent;
        friend class IDisp_DetachEvent;

    protected:
        FB::JSAPIWeakPtr m_api;
        ActiveXBrowserHostWeakPtr m_host;
		void JSAPI_IDispatchExBase::setPromise(FB::variantPromise promise, VARIANT *result) {
			auto comPromise = ComPromise::create(getHost(), promise);

			CComVariant outVar;
			::VariantInit(result);

			outVar = comPromise->getComPromise()->getIDispatch();
			outVar.Detach(result);
		}
    };

    template <class T, class IDISP, const IID* piid>
    class JSAPI_IDispatchEx :
        public JSAPI_IDispatchExBase,
        public IDISP,
        public IConnectionPointContainer,
        public IConnectionPoint
    {
        using CComEnumConnectionPoints = CComEnum < IEnumConnectionPoints, &__uuidof(IEnumConnectionPoints), IConnectionPoint*, _CopyInterface<IConnectionPoint> > ;
        using ConnectionPointMap = std::map < DWORD, IDispatchAPIPtr > ;

    public:
        JSAPI_IDispatchEx(std::string mimetype) : m_mimetype(mimetype), m_readyState(READYSTATE_LOADING) { };
        virtual ~JSAPI_IDispatchEx(void) { };
        void setReadyState(READYSTATE newState)
        {
            m_readyState = newState;
            if (m_propNotify)
                m_propNotify->OnChanged(DISPID_READYSTATE);
        }

    private:
        const std::string m_mimetype;

    protected:
        ConnectionPointMap m_connPtMap;

        READYSTATE m_readyState;
        CComQIPtr<IPropertyNotifySink, &IID_IPropertyNotifySink> m_propNotify;
        
        bool m_valid;
        std::vector<std::wstring> m_memberList;

        virtual bool callSetEventListener(const std::vector<FB::variant> &args, bool add);

    public:
        /* IConnectionPointContainer members */
        HRESULT STDMETHODCALLTYPE EnumConnectionPoints(IEnumConnectionPoints **ppEnum);
        HRESULT STDMETHODCALLTYPE FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP);

        /* IConnectionPoint members */
        HRESULT STDMETHODCALLTYPE GetConnectionInterface(IID *pIID);
        HRESULT STDMETHODCALLTYPE GetConnectionPointContainer(IConnectionPointContainer **ppCPC);
        HRESULT STDMETHODCALLTYPE Advise(IUnknown *pUnkSink, DWORD *pdwCookie);
        HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie);
        HRESULT STDMETHODCALLTYPE EnumConnections(IEnumConnections **ppEnum);

        /* IDispatch members */
        HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
        HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
        HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid,
            DISPID *rgDispId);
        HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
            DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

        /* IDispatchEx members */
        HRESULT STDMETHODCALLTYPE GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid);
        HRESULT STDMETHODCALLTYPE InvokeEx(DISPID id, LCID lcid, WORD wFlags,
            DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller);
        HRESULT STDMETHODCALLTYPE DeleteMemberByName(BSTR bstrName, DWORD grfdex);
        HRESULT STDMETHODCALLTYPE DeleteMemberByDispID(DISPID id);
        HRESULT STDMETHODCALLTYPE GetMemberProperties(DISPID id, DWORD grfdexFetch,
            DWORD *pgrfdex);
        HRESULT STDMETHODCALLTYPE GetMemberName(DISPID id, BSTR *pbstrName);
        HRESULT STDMETHODCALLTYPE GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid);
        HRESULT STDMETHODCALLTYPE GetNameSpaceParent(IUnknown **ppunk);
    };

    /* IConnectionPointContainer methods */
    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::EnumConnectionPoints(IEnumConnectionPoints **ppEnum)
    {
        if (!ppEnum)
            return E_POINTER;
        *ppEnum = nullptr;
        CComEnumConnectionPoints* pEnum = nullptr;

        pEnum = new CComObject<CComEnumConnectionPoints>;
        if (!pEnum)
            return E_OUTOFMEMORY;

        IConnectionPoint *connectionPoint[1] = { nullptr };
        static_cast<T*>(this)->QueryInterface(IID_IConnectionPoint, (void **)connectionPoint);

        HRESULT hRes = pEnum->Init(connectionPoint, &connectionPoint[1],
            reinterpret_cast<IConnectionPointContainer*>(this), AtlFlagCopy);

        connectionPoint[0]->Release();
        
        if (FAILED(hRes))
        {
            delete pEnum;
            return hRes;
        }
        hRes = pEnum->QueryInterface(__uuidof(IEnumConnectionPoints), (void**)ppEnum);
        if (FAILED(hRes))
            delete pEnum;
        return hRes;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP)
    {
        if (!ppCP)
            return E_POINTER;
        *ppCP = nullptr;
        HRESULT hRes = CONNECT_E_NOCONNECTION;

        if (InlineIsEqualGUID(*piid, riid)) {
            hRes = static_cast<T*>(this)->QueryInterface(__uuidof(IConnectionPoint), (void**)ppCP);
        }

        return hRes;
    }

    /* IConnectionPoint methods */
    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetConnectionInterface(IID *pIID)
    {
        *pIID = *piid;
        return S_OK;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetConnectionPointContainer(IConnectionPointContainer **ppCPC)
    {
        if (!ppCPC)
            return E_POINTER;

        return static_cast<T*>(this)->QueryInterface(__uuidof(IConnectionPointContainer), (void**)ppCPC);
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::Advise(IUnknown *pUnkSink, DWORD *pdwCookie)
    {
        try {
            CComQIPtr<IDispatch> disp(pUnkSink);
            if (disp) {
                IDispatchAPIPtr obj(IDispatchAPI::create(disp, getHost()));
                m_connPtMap[(DWORD)obj.get()] = obj;
                *pdwCookie = (DWORD)obj.get();
                getAPI()->registerEventInterface(obj);
                return S_OK;
            } else {
                return CONNECT_E_CANNOTCONNECT;
            }
        } catch (const std::exception&) {
            return CONNECT_E_CANNOTCONNECT;
        }
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::Unadvise(DWORD dwCookie)
    {
        try {
            ConnectionPointMap::iterator fnd = m_connPtMap.find(dwCookie);
            if (fnd == m_connPtMap.end()) {
                return E_UNEXPECTED;
            } else {
                getAPI()->unregisterEventInterface(fnd->second);
                m_connPtMap.erase(fnd);
                return S_OK;
            }
        } catch (const std::exception&) {
            return S_OK;
        }
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::EnumConnections(IEnumConnections **ppEnum)
    {
        return E_NOTIMPL;
    }

    /* IDispatch methods */
    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetTypeInfoCount(UINT *pctinfo)
    {
        return E_NOTIMPL;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
    {
        return E_NOTIMPL;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, 
                                                      UINT cNames, LCID lcid, 
                                                      DISPID *rgDispId)
    {
        HRESULT rv = S_OK;
        for (UINT i = 0; i < cNames; i++) {
            CComBSTR bstr(rgszNames[i]);
            rv = GetDispID(bstr, 0, &rgDispId[i]);
            if (!SUCCEEDED(rv)) {
                return rv;
            }
        }
        return rv;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                                               WORD wFlags, DISPPARAMS *pDispParams, 
                                               VARIANT *pVarResult, EXCEPINFO *pExcepInfo,
                                               UINT *puArgErr)
    {
        return InvokeEx(dispIdMember, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, nullptr);
    }


    /* IDispatchEx members */
    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
    {
        std::wstring wsName(bstrName);

        try {
            FB::JSAPIPtr api(getAPI());
            if ((wsName == L"attachEvent") || (wsName == L"detachEvent") || (wsName == L"getLastException")) {
                *pid = AxIdMap.getIdForValue(wsName);
            } else if (api->HasProperty(wsName) || api->HasMethod(wsName)) {
                *pid = AxIdMap.getIdForValue(wsName);
            } else {
                *pid = -1;
                return DISP_E_UNKNOWNNAME;
            }
            return S_OK;
        } catch (const std::exception &e) {
            e;
            *pid = -1;
            return DISP_E_UNKNOWNNAME;
        }
    }

    // helper method for Invoke
    template <class T, class IDISP, const IID* piid>
    bool JSAPI_IDispatchEx<T,IDISP,piid>::callSetEventListener(const std::vector<FB::variant> &args, bool add)
    {
        if (args.size() < 2 || args.size() > 3) {
            throw FB::invalid_arguments();
        }
        
        try {
            std::string evtName = args[0].convert_cast<std::string>();
            FB::JSObjectPtr method(args[1].convert_cast<FB::JSObjectPtr>());
            if (add) {
                getAPI()->registerEventMethod(evtName, method);
            } else {
                getAPI()->unregisterEventMethod(evtName, method);
            }
        } catch(const std::bad_cast&) {
            throw FB::invalid_arguments();
        }

        return true;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::InvokeEx(DISPID id, LCID lcid, WORD wFlags,
                                                 DISPPARAMS *pdp, VARIANT *pvarRes, 
                                                 EXCEPINFO *pei, 
                                                 IServiceProvider *pspCaller)
    {
        FB::JSAPIPtr api;
        try {
            api = getAPI();
        } catch (...) {
            return DISP_E_MEMBERNOTFOUND;
        }
        if (!AxIdMap.idExists(id)) {
            return DISP_E_MEMBERNOTFOUND;
        }
        
        std::wstring wsName;
        
        try 
        {
            wsName = AxIdMap.getValueForId<std::wstring>(id);
            ActiveXBrowserHostPtr host(getHost());
            
            if (wFlags & DISPATCH_PROPERTYGET) {
                if(!pvarRes)
                    return E_INVALIDARG;
                    
                switch(id) {
                    case DISPID_READYSTATE:
                        CComVariant(this->m_readyState).Detach(pvarRes);
                        return S_OK;
                    case DISPID_ENABLED:
                        CComVariant(true).Detach(pvarRes);
                        return S_OK;
                    case DISPID_SECURITYCTX:
                        return E_NOTIMPL;
                }
            }
            
            if (wFlags & DISPATCH_METHOD && (api->HasMethod(wsName) || !id) ) {
                
                std::vector<FB::variant> params;
                if (pdp->cNamedArgs > 0 && pdp->rgdispidNamedArgs[0] == DISPID_THIS) {
                    if (id == 0)
                        wsName = L"";
                    for (int i = pdp->cArgs - 1; i >= 1; i--) {
                        params.emplace_back(host->getVariant(&pdp->rgvarg[i]));
                    }
                } else {
                    for (int i = pdp->cArgs - 1; i >= 0; i--) {
                        params.emplace_back(host->getVariant(&pdp->rgvarg[i]));
                    }
                }
                FB::variantPromise rVal;
                rVal = api->Invoke(wsName, params);
		
                if(pvarRes)
                    setPromise(rVal, pvarRes);
                
            } else if (wFlags & DISPATCH_PROPERTYGET && api->HasMethod(wsName)) {
                
                FB::variant rVal;
                rVal = true;
                host->getComVariant(pvarRes, rVal);
                
            } else if (wFlags & DISPATCH_PROPERTYGET && api->HasProperty(wsName)) {
                
                if(!pvarRes)
                    return E_INVALIDARG;
                    
                    setPromise(api->GetProperty(wsName), pvarRes);
                    
            } else if ((wFlags & DISPATCH_PROPERTYPUT || wFlags & DISPATCH_PROPERTYPUTREF) && api->HasProperty(wsName)) {
                
                FB::variant newVal = host->getVariant(&pdp->rgvarg[0]);
                api->SetProperty(wsName, newVal);
                
            } else {
                throw FB::invalid_member("Invalid method or property name");
            }
            
            return S_OK;
        } catch (const std::exception &e) {
            auto ep = std::current_exception();
            try {
                FB::variantDeferred dfd;
                dfd.reject(ep);
                setPromise(dfd.promise(), pvarRes);
                return S_OK;
            }
            catch (...) {
                if (pei) {
                    pei->bstrSource = CComBSTR(m_mimetype.c_str()).Detach();
                    pei->bstrDescription = CComBSTR(e.what()).Detach();
                    pei->bstrHelpFile = nullptr;
                    pei->pfnDeferredFillIn = nullptr;
                    pei->scode = E_NOTIMPL;
                }
                return DISP_E_EXCEPTION;
            }
        }
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
    {
        return E_NOTIMPL;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::DeleteMemberByDispID(DISPID id)
    {
        FB::JSAPIPtr api;
        try {
            api = getAPI();
        } catch (...) {
            return S_FALSE;
        }
        if (!AxIdMap.idExists(id)) {
            return S_FALSE;
        }

        std::wstring wsName;
        try 
        {
            wsName = AxIdMap.getValueForId<std::wstring>(id);
            api->RemoveProperty(wsName);
        } catch (const FB::script_error& se) {
            FBLOG_INFO("JSAPI_IDispatchEx", "Script error for \"" << FB::wstring_to_utf8(wsName) << "\": " << se.what());
            return S_FALSE;
        } catch (...) {
            return S_FALSE;
        }

        return S_OK;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetMemberProperties(DISPID id, DWORD grfdexFetch,
                                                            DWORD *pgrfdex)
    {
        return E_NOTIMPL;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetMemberName(DISPID id, BSTR *pbstrName)
    {
        try {
            CComBSTR outStr(AxIdMap.getValueForId<std::wstring>(id).c_str());

            *pbstrName = outStr.Detach();
            return S_OK;
        } catch (...) {
            return DISP_E_UNKNOWNNAME;
        }
        return E_NOTIMPL;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
    {
        FB::JSAPIPtr api;
        try {
            api = getAPI();
        } catch (...) {
            return S_FALSE;
        }

        if (m_memberList.size() != api->getMemberCount()) {
            m_memberList.clear();
            api->getMemberNames(m_memberList);
            m_memberList.emplace_back(L"attachEvent");
            m_memberList.emplace_back(L"detachEvent");
			m_memberList.emplace_back(L"getLastException");
        }
        if (m_memberList.size() == 0)
            return S_FALSE;

        if (id == DISPID_STARTENUM) {
            *pid = AxIdMap.getIdForValue(m_memberList[0]);
            return S_OK;
        } 
        std::wstring wStr = AxIdMap.getValueForId<std::wstring>(id);

        std::vector<std::wstring>::iterator it;
        for (it = m_memberList.begin(); it != m_memberList.end(); it++) {
            if (wStr == *it) {
                it++;
                break;
            }
        }
        if (it != m_memberList.end()) {
            *pid = AxIdMap.getIdForValue(*it);
        } else {
            return S_FALSE;
        }
        return S_OK;
    }

    template <class T, class IDISP, const IID* piid>
    HRESULT JSAPI_IDispatchEx<T,IDISP,piid>::GetNameSpaceParent(IUnknown **ppunk)
    {
        return E_NOTIMPL;
    }
} }
#endif // H_JSAPI_IDISPATCHEX
