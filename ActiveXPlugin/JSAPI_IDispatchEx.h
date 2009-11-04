/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_JSAPI_IDISPATCHEX
#define H_JSAPI_IDISPATCHEX

#include "ActiveXBrowserHost.h"
#include "APITypes.h"
#include "JSAPI.h"
#include "TypeIDMap.h"
#include "COM_config.h"

#include "dispex.h"

template <class T>
class JSAPI_IDispatchEx :
    public T
{
public:
    JSAPI_IDispatchEx(void) : m_idMap(100) { };
    virtual ~JSAPI_IDispatchEx(void) { };
    void setAPI(FB::JSAPI *api, ActiveXBrowserHost *host)
    {
        m_api = api;
        m_host = host;
    }

protected:
    FB::AutoPtr<FB::JSAPI> m_api;
    FB::AutoPtr<ActiveXBrowserHost> m_host;
    FB::TypeIDMap<DISPID> m_idMap;
    bool m_valid;

public:
    /* IDispatch members */
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames,
        LCID lcid, DISPID *rgDispId);
    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid,
        LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult,
        EXCEPINFO *pExcepInfo, UINT *puArgErr);

    /* IDispatchEx members */
    virtual HRESULT STDMETHODCALLTYPE GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid);
    virtual HRESULT STDMETHODCALLTYPE InvokeEx(DISPID id, LCID lcid, WORD wFlags,
        DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller);
    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByName(BSTR bstrName, DWORD grfdex);
    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByDispID(DISPID id);
    virtual HRESULT STDMETHODCALLTYPE GetMemberProperties(DISPID id, DWORD grfdexFetch,
        DWORD *pgrfdex);
    virtual HRESULT STDMETHODCALLTYPE GetMemberName(DISPID id, BSTR *pbstrName);
    virtual HRESULT STDMETHODCALLTYPE GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid);
    virtual HRESULT STDMETHODCALLTYPE GetNameSpaceParent(IUnknown **ppunk);
};

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetTypeInfoCount(UINT *pctinfo)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetTypeInfo(UINT iTInfo, LCID lcid, 
                                                            ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, 
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

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                                                       WORD wFlags, DISPPARAMS *pDispParams, 
                                                       VARIANT *pVarResult, EXCEPINFO *pExcepInfo,
                                                       UINT *puArgErr)
{
    return InvokeEx(dispIdMember, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, NULL);
}


/* IDispatchEx members */
template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
    std::string sName(CW2A(bstrName).m_psz);
    
    if (m_api->HasProperty(sName) || m_api->HasMethod(sName)) {
        *pid = m_idMap.getIdForValue(sName);
    } else {
        *pid = -1;
    }
    return S_OK;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::InvokeEx(DISPID id, LCID lcid, WORD wFlags,
                                                         DISPPARAMS *pdp, VARIANT *pvarRes, 
                                                         EXCEPINFO *pei, 
                                                         IServiceProvider *pspCaller)
{
    if (!m_idMap.idExists(id)) {
        return E_NOTIMPL;
    }
    try {
        std::string sName = m_idMap.getValueForId<std::string>(id);

        if (wFlags & DISPATCH_PROPERTYGET && m_api->HasProperty(sName)) {

            FB::variant rVal = m_api->GetProperty(sName);

            m_host->getComVariant(pvarRes, rVal);

        } else if (wFlags & DISPATCH_PROPERTYPUT && m_api->HasProperty(sName)) {

            FB::variant newVal = m_host->getVariant(&pdp->rgvarg[0]);

            m_api->SetProperty(sName, newVal);

        } else if (wFlags & DISPATCH_METHOD && m_api->HasMethod(sName)) {

            std::vector<FB::variant> params;
            for (int i = pdp->cArgs; i > 0; --i) {
                params.push_back(m_host->getVariant(&pdp->rgvarg[i]));
            }
            FB::variant rVal = m_api->Invoke(sName, params);

            m_host->getComVariant(pvarRes, rVal);

        } else {
            throw FB::script_error("Invalid method or property name");
        }
    } catch (FB::script_error se) {
        pei->bstrSource = CComBSTR(ACTIVEX_PROGID);
        pei->bstrDescription = CComBSTR(se.what());
        pei->bstrHelpFile = NULL;
        pei->pfnDeferredFillIn = NULL;
        pei->scode = E_NOTIMPL;
        return DISP_E_EXCEPTION;
    } catch (...) {
        return E_NOTIMPL;
    }

    return S_OK;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::DeleteMemberByDispID(DISPID id)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetMemberProperties(DISPID id, DWORD grfdexFetch,
                                                                    DWORD *pgrfdex)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetMemberName(DISPID id, BSTR *pbstrName)
{
    try {
        CComBSTR outStr(m_idMap.getValueForId<std::string>(id).c_str());

        *pbstrName = outStr.Detach();
        return S_OK;
    } catch (...) {
        return DISP_E_UNKNOWNNAME;
    }
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetNameSpaceParent(IUnknown **ppunk)
{
    return E_NOTIMPL;
}

#endif // H_JSAPI_IDISPATCHEX