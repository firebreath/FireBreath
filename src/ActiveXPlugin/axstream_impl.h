/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_ACTIVEXSTREAM_IMPL
#define H_ACTIVEXSTREAM_IMPL

#include "BrowserStream.h"
#include "urlmon.h"
#include "atlbase.h"

// includes parts of Microsoft examples:
//   http://support.microsoft.com/kb/223500
//   http://support.microsoft.com/kb/165800

class ActiveXStream;
class ActiveXBindStatusCallback;

class ActiveXStreamRequest
{
public:
    ActiveXStreamRequest( ActiveXStream* stream );
    ActiveXStreamRequest( ActiveXStream* stream, const std::vector<FB::BrowserStream::Range>& ranges );

    bool start();
    bool stop(bool streamDetached = false);

public:
    ActiveXStream*		stream;
    CComPtr<IMoniker>	FMoniker;
    CComPtr<IBindCtx>	FBindCtx;
    CComPtr<ActiveXBindStatusCallback> bindStatusCallback;
    CComPtr<IStream>	fstream;
    std::vector<FB::BrowserStream::Range>	ranges;
};

class ActiveXBindStatusCallback : public IBindStatusCallback, IHttpNegotiate 
{
  public:
    // IUnknown methods
    STDMETHODIMP    QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef()    { return m_cRef++; }
    STDMETHODIMP_(ULONG)    Release()   { if (--m_cRef == 0) { delete this; return 0; } return m_cRef; }

    // IBindStatusCallback methods
    STDMETHODIMP    OnStartBinding(DWORD dwReserved, IBinding* pbinding);
    STDMETHODIMP    GetPriority(LONG* pnPriority);
    STDMETHODIMP    OnLowResource(DWORD dwReserved);
    STDMETHODIMP    OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode,
                        LPCWSTR pwzStatusText);
    STDMETHODIMP    OnStopBinding(HRESULT hrResult, LPCWSTR szError);
    STDMETHODIMP    GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindinfo);
    STDMETHODIMP    OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pfmtetc,
                        STGMEDIUM* pstgmed);
    STDMETHODIMP    OnObjectAvailable(REFIID riid, IUnknown* punk);

    // IHttpNegotiate methods
    STDMETHODIMP BeginningTransaction(/* [in] */ LPCWSTR szURL,
                    /* [unique][in] */ LPCWSTR szHeaders,
                    /* [in] */ DWORD dwReserved,
                    /* [out] */ LPWSTR __RPC_FAR *pszAdditionalHeaders);
        
    STDMETHODIMP OnResponse(/* [in] */ DWORD dwResponseCode,
        /* [unique][in] */ LPCWSTR szResponseHeaders,
        /* [unique][in] */ LPCWSTR szRequestHeaders,
        /* [out] */ LPWSTR __RPC_FAR *pszAdditionalRequestHeaders);

    // constructors/destructors
    ActiveXBindStatusCallback();
    virtual
    ~ActiveXBindStatusCallback();

    static HRESULT Create(ActiveXBindStatusCallback** ppBindStatusCallback, ActiveXStreamRequest* request);
    HRESULT Init(ActiveXStreamRequest* request);

    bool close();

    bool GetInfo(DWORD which, std::string& result);

    // data members
    DWORD           m_cRef;
    LPBINDING       m_pbinding;
    LPSTREAM        m_pstm;
    DWORD           m_cbOld;
    BOOL			m_fRedirect; // need to be informed when we're being redirected by the server
    BINDVERB		m_dwAction;
    ActiveXStreamRequest*	m_request;
    bool			m_transactionStarted;
};

#endif