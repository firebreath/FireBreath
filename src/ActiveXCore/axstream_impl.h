/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_ACTIVEXSTREAM_IMPL
#define H_ACTIVEXSTREAM_IMPL

#include "BrowserStream.h"
#include "urlmon.h"
#include "atlbase.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// includes parts of Microsoft examples:
//   http://support.microsoft.com/kb/223500
//   http://support.microsoft.com/kb/165800

namespace FB { namespace ActiveX {
    class ActiveXStream;
    typedef boost::shared_ptr<ActiveXStream> ActiveXStreamPtr;
    class ActiveXBindStatusCallback;

    class ActiveXStreamRequest : public boost::enable_shared_from_this<ActiveXStreamRequest>
    {
    public:
        ActiveXStreamRequest( ActiveXStreamPtr stream );
        ActiveXStreamRequest( ActiveXStreamPtr stream, const std::vector<FB::BrowserStream::Range>& ranges );

        bool start();
        bool stop();

    public:
        ActiveXStreamPtr      stream;
        CComPtr<IMoniker>   FMoniker;
        CComPtr<IBindCtx>   FBindCtx;
        CComPtr<ActiveXBindStatusCallback> bindStatusCallback;
        CComPtr<IStream>    fstream;
        std::vector<FB::BrowserStream::Range>   ranges;
    };

    typedef boost::shared_ptr<ActiveXStreamRequest> ActiveXStreamRequestPtr;

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

        static HRESULT Create(ActiveXBindStatusCallback** ppBindStatusCallback, ActiveXStreamRequestPtr request);
        HRESULT Init(ActiveXStreamRequestPtr request);
        HRESULT InitPostData(const char* szData);

        bool close();

        bool GetInfo(DWORD which, std::string& result);

        // data members
        DWORD           m_cRef;
        LPBINDING       m_pbinding;
        CComPtr<IStream> m_pstm;
        DWORD           m_cbOld;
        BOOL            m_fRedirect; // need to be informed when we're being redirected by the server
        BINDVERB        m_dwAction;
        HGLOBAL         m_hDataToPost; // data that we're going to post
        DWORD           m_cbDataToPost;
        ActiveXStreamRequestPtr m_request;
        bool            m_transactionStarted;
    };
} }

#endif

