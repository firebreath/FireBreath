
#ifndef H_JSAPI_IDISPATCHEX
#define H_JSAPI_IDISPATCHEX

#include "APITypes.h"
#include "JSAPI.h"

#include "dispex.h"

class JSAPI_IDispatchEx :
    public IDispatchEx
{
public:
    JSAPI_IDispatchEx(void);
    virtual ~JSAPI_IDispatchEx(void);

protected:
    FB::AutoPtr<FB::JSAPI> m_api;
    FB::AutoPtr<NpapiBrowserHost> m_browser;
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


#endif // H_JSAPI_IDISPATCHEX