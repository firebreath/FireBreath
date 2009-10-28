#include "JSAPI_IDispatchEx.h"

JSAPI_IDispatchEx::JSAPI_IDispatchEx(void)
{
}

JSAPI_IDispatchEx::~JSAPI_IDispatchEx(void)
{
}


HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetTypeInfoCount(UINT *pctinfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    return E_NOTIMPL;
}


/* IDispatchEx members */
HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::DeleteMemberByName(BSTR bstrName, DWORD grfdex)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::DeleteMemberByDispID(DISPID id)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetMemberName(DISPID id, BSTR *pbstrName)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE JSAPI_IDispatchEx::GetNameSpaceParent(IUnknown **ppunk)
{
    return E_NOTIMPL;
}

