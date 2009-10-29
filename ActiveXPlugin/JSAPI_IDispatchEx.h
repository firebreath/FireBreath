
#ifndef H_JSAPI_IDISPATCHEX
#define H_JSAPI_IDISPATCHEX

#include "APITypes.h"
#include "JSAPI.h"
#include "ActiveXBrowserHost.h"

#include "dispex.h"

template <class T>
class JSAPI_IDispatchEx :
    public T
{
public:
    JSAPI_IDispatchEx(void) { };
    virtual ~JSAPI_IDispatchEx(void) { };

protected:
    FB::AutoPtr<FB::JSAPI> m_api;
    FB::AutoPtr<ActiveXBrowserHost> m_browser;
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
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    return E_NOTIMPL;
}


/* IDispatchEx members */
template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
    return E_NOTIMPL;
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
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
    return E_NOTIMPL;
}

template <class T>
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx<T>::GetMemberName(DISPID id, BSTR *pbstrName)
{
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