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

// includes parts of Microsoft examples:
//   http://support.microsoft.com/kb/223500
//   http://support.microsoft.com/kb/165800

#include "axstream_impl.h"
#include "axstream.h"
#include <tchar.h>
#include <sstream>
#include "wininet.h"
#include "utf8_tools.h"
#include <boost\smart_ptr\scoped_array.hpp>
#include <boost\smart_ptr\shared_array.hpp>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;
using namespace FB::ActiveX;

#define ODS(x)

// ===========================================================================
//                     ActiveXBindStatusCallback Implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::ActiveXBindStatusCallback
// ---------------------------------------------------------------------------
ActiveXBindStatusCallback::ActiveXBindStatusCallback() :
    m_pbinding(0), m_pstm(0), m_cRef(1), m_cbOld(0), m_dwAction( BINDVERB_GET ), m_fRedirect( FALSE ), m_transactionStarted( false ),
    m_hDataToPost(NULL), m_cbDataToPost(0) 
{
}

ActiveXBindStatusCallback::~ActiveXBindStatusCallback()
{
    if (m_hDataToPost) 
    {
        ::GlobalFree(m_hDataToPost);
        m_hDataToPost = NULL;
    }
}

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::Create
// ---------------------------------------------------------------------------
HRESULT ActiveXBindStatusCallback::Create(ActiveXBindStatusCallback** ppBindStatusCallback, ActiveXStreamRequestPtr request)
{
    HRESULT hr;
    ActiveXBindStatusCallback* pBindStatusCallback;

    if (!ppBindStatusCallback)
    {
        return E_POINTER;
    }
    
    *ppBindStatusCallback = NULL;

    pBindStatusCallback = new ActiveXBindStatusCallback();
    if (!pBindStatusCallback)
    {
        return E_OUTOFMEMORY;
    }

    hr = pBindStatusCallback->Init(request);
    if (FAILED(hr))
    {
        delete pBindStatusCallback;
        return hr;
    }

    *ppBindStatusCallback = pBindStatusCallback;
    return NOERROR;
}

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::Init
// ---------------------------------------------------------------------------
HRESULT ActiveXBindStatusCallback::Init(ActiveXStreamRequestPtr request)
{
    HRESULT hr = NOERROR;
    m_request = request;

    if(     m_request->stream
        &&  !m_request->stream->getVerbData().empty()   )
    {
        // Is a post request
        m_dwAction = BINDVERB_POST;
        hr = InitPostData(m_request->stream->getVerbData().c_str());
    }

    m_dwAction = m_request->stream->getVerbData().empty()? BINDVERB_GET : BINDVERB_POST;
    return hr;
}

HRESULT ActiveXBindStatusCallback::InitPostData(const char* szData)
{
    if (m_hDataToPost)
    {
        // We're already in use and don't support recycling the same instance
        // Some other client may have a reference on us.
        // If we were to free this data, the client might crash dereferencing the handle
        return E_FAIL; 
    }

    if (szData)
    {
        // MSINTERNAL: See CINetHttp::INetAsyncSendRequest (cnethttp.cxx) that URLMON calls CINetHttp::GetDataToSend() followed by a call to WININET's HttpSendRequest(). GetDataToSend essentially pulls the data out of the BINDINFO that URLMON has cached away when it calls the host's implementation of IBindStatusCallback::GetBindInfo(). 
        // MSINTERNAL: It doesn't attempt to lock down the HGLOBAL at all, so we need to allocated GMEM_FIXED
        m_cbDataToPost = ::lstrlenA(szData);
        m_hDataToPost = ::GlobalAlloc(GPTR, m_cbDataToPost+1); // GMEM_MOVEABLE won't work because URLMON doesn't attempt GlobalLock before dereferencing
        if (!m_hDataToPost)
        {
            return E_OUTOFMEMORY;
        }

        // the memory was allocate fixed, so no need to lock it down
        ::lstrcpyA((char*)m_hDataToPost, szData);
    }
    
    return NOERROR;
}

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::QueryInterface
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::QueryInterface(REFIID riid, void** ppv)
{
    *ppv = NULL;

    if (riid==IID_IUnknown || riid==IID_IBindStatusCallback)    
    {
        *ppv = (IBindStatusCallback*)this;
        AddRef();
        return S_OK;
    }
    else if (riid==IID_IHttpNegotiate)
    {
        *ppv = (IHttpNegotiate*)this;
        AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }

}  // ActiveXBindStatusCallback::QueryInterface

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnStartBinding
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding* pbinding)
{
    if (NULL != m_pbinding)
    {
        m_pbinding->Release();
    }

    // Cache the URLMON-provided IBinding interface so that we can control the download
    m_pbinding = pbinding;

    if (m_pbinding != NULL)
    {
        m_pbinding->AddRef();
        //SetStatusText(_T("Starting to bind..."));
    }

    return S_OK;
}  // ActiveXBindStatusCallback::OnStartBinding

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::GetPriority
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::GetPriority(LONG* pnPriority)
{
    return E_NOTIMPL;
}  // ActiveXBindStatusCallback::GetPriority

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnLowResource
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::OnLowResource(DWORD dwReserved)
{
    return E_NOTIMPL;
}  // ActiveXBindStatusCallback::OnLowResource

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnProgress
// ---------------------------------------------------------------------------
STDMETHODIMP ActiveXBindStatusCallback::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
    //if ( ulProgressMax && m_request->stream ) m_request->stream->length = ulProgressMax;

    // for more see here http://msdn.microsoft.com/en-us/library/ms775133(VS.85).aspx
    switch (ulStatusCode)
    {
    case BINDSTATUS_REDIRECTING: 
        ODS("Server redirecting client\n");
        m_fRedirect = TRUE;
        break;
    case BINDSTATUS_FINDINGRESOURCE:
        ODS("Finding resource\n");
        break;
    case BINDSTATUS_CONNECTING:
        ODS("Connecting...\n");
        break;
    case BINDSTATUS_BEGINDOWNLOADDATA:
        ODS("Beginning to download data...\n");
        break;
    case BINDSTATUS_DOWNLOADINGDATA:
        ODS("Downloading data...\n");
        break;
    case BINDSTATUS_ENDDOWNLOADDATA:
        ODS("Ending data download...\n");
        break;
    case BINDSTATUS_BEGINDOWNLOADCOMPONENTS:
        ODS("Beginning to download components...\n");
        break;
    case BINDSTATUS_INSTALLINGCOMPONENTS:
        ODS("Installing components...\n");
        break;
    case BINDSTATUS_ENDDOWNLOADCOMPONENTS:
        ODS("Ending component download...\n");
        break;
    case BINDSTATUS_USINGCACHEDCOPY:
        ODS("Using cached copy...\n");
        break;
    case BINDSTATUS_SENDINGREQUEST:
        ODS("Sending request...\n");
        break;
    case BINDSTATUS_CLASSIDAVAILABLE:
        ODS("CLSID available...\n");
        break;
    case BINDSTATUS_MIMETYPEAVAILABLE:
        ODS("MIME type available...\n");
        break;
    case BINDSTATUS_CACHEFILENAMEAVAILABLE:
        ODS("Cache file name available...\n");
        if ( m_request->stream ) m_request->stream->signalCacheFilename( szStatusText );
        break;
    // also see OnResponse for these two
    case BINDSTATUS_RAWMIMETYPE:
        // never called?
        //if ( m_request->stream ) m_request->stream->mimeType = FB::wstring_to_utf8( szStatusText );
        break;
    case BINDSTATUS_ACCEPTRANGES:
        // never called?
        //if ( m_request->stream ) m_request->stream->seekable = true;
        break;
    default:
        ODS("Unknown binding status code!\n");
        break;
    }

    //wsprintf(szProgress, "%d of %d", ulProgress, (ulProgress>ulProgressMax)?ulProgress:ulProgressMax);

    //SetStatusText(szProgress, SBAR_PART_PROGRESS);
    //UpdateProgress(ulProgress, ulProgressMax);

    return NOERROR;
}  // ActiveXBindStatusCallback::OnProgress

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnStopBinding
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::OnStopBinding(HRESULT hrStatus, LPCWSTR pszError)
{
    if ( m_request->stream ) m_request->stream->signalRequestCompleted( m_request, !FAILED(hrStatus) );

    if (m_pbinding)
    {
        m_pbinding->Release();
        m_pbinding = NULL;
    }

    ODS("OnStopBinding\n");

    return S_OK;
}  // ActiveXBindStatusCallback::OnStopBinding

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::GetBindInfo
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindInfo)
{
    *pgrfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA;
    if ( m_request->stream && m_request->stream->isCached() ) *pgrfBINDF |= BINDF_NEEDFILE;
    else *pgrfBINDF |= BINDF_NOWRITECACHE | BINDF_PRAGMA_NO_CACHE | BINDF_GETNEWESTVERSION;

    // Set up the BINDINFO data structure
    pbindInfo->cbSize = sizeof(BINDINFO);
    pbindInfo->dwBindVerb = m_dwAction; // here's where the action is defined
    pbindInfo->szExtraInfo = NULL;

    // Initialize the STGMEDIUM.
    memset(&pbindInfo->stgmedData, 0, sizeof(STGMEDIUM));
    pbindInfo->grfBindInfoF = 0;
    pbindInfo->szCustomVerb = NULL;

    // set up action-specific members
    switch(m_dwAction)
    {
    case BINDVERB_POST:
        if (m_hDataToPost)
        {           
            // Fill the STGMEDIUM with the data to post
            pbindInfo->stgmedData.tymed = TYMED_HGLOBAL;    // this is the only medium urlmon supports right now
            pbindInfo->stgmedData.hGlobal = m_hDataToPost;
            pbindInfo->stgmedData.pUnkForRelease = (LPUNKNOWN)(LPBINDSTATUSCALLBACK)this; //  maintain control over the data. 
            AddRef();   // It will be freed on final release
            pbindInfo->cbstgmedData =   // this must be exact! 
                m_cbDataToPost;         // Do not rely on GlobalSize() 
                                        // which rounds up to the nearest power of two.
        }
        break;
    case BINDVERB_GET:
        break;
    default:
        return E_FAIL;
    }

    return S_OK;
}  // ActiveXBindStatusCallback::GetBindInfo

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnDataAvailable
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pfmtetc, STGMEDIUM* pstgmed)
{
     HRESULT hr = S_OK;

    if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
    {
        // Cache the incoming Stream
        if (!m_pstm && pstgmed->tymed == TYMED_ISTREAM)
        {
            m_pstm = pstgmed->pstm;
        }
    }

    // If there is some data to be read then go ahead and read it
    if (m_pstm && dwSize > m_cbOld)
    {
        DWORD dwRead = dwSize - m_cbOld;    // Minimum amount available that hasn't been read
        DWORD dwActuallyRead = 0;           // Placeholder for amount read during this pull

        DWORD offset = 0;
        std::string data;
        if ( GetInfo( HTTP_QUERY_CONTENT_RANGE, data ) )        // data look like bytes 0-3/4234
        {
            size_t endPos = data.find( "-" );
            if ( endPos != std::string::npos )
            {
                size_t startPos = 6;        // "bytes "
                offset = atol( data.substr(startPos, endPos - startPos).c_str() );
            }
        }

        if (dwRead > 0)
        do
        {
            boost::shared_array<char> cur(new char[dwRead]);

            hr = m_pstm->Read( cur.get(), dwRead, &dwActuallyRead);

            // If we really read something then lets add it to the Edit box
            if (dwActuallyRead > 0)
            {
                if ( m_request->stream ) m_request->stream->signalDataArrived( cur.get(), dwActuallyRead, offset + m_cbOld );
                m_cbOld += dwActuallyRead;
            }

        } while (!(hr == E_PENDING || hr == S_FALSE) && SUCCEEDED(hr));
    }

    // Clean up
    if (BSCF_LASTDATANOTIFICATION & grfBSCF)
    {
        if (m_pstm)
        {
            m_pstm.Release();
        }

        hr = S_OK;  // If it was the last data then we should return S_OK as we just finished reading everything

        //SetStatusText(_T("File downloaded."));

        ODS("OnProgress: Last notification\n");
    }

    return hr;
}  // ActiveXBindStatusCallback::OnDataAvailable

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnObjectAvailable
// ---------------------------------------------------------------------------
 STDMETHODIMP
ActiveXBindStatusCallback::OnObjectAvailable(REFIID riid, IUnknown* punk)
{
    return E_NOTIMPL;
}  // ActiveXBindStatusCallback::OnObjectAvailable

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::BeginningTransaction
// ---------------------------------------------------------------------------
STDMETHODIMP ActiveXBindStatusCallback::BeginningTransaction(LPCWSTR szURL,
                    LPCWSTR szHeaders,
                    DWORD dwReserved,
                    LPWSTR __RPC_FAR *pszAdditionalHeaders)
{
    // Here's our opportunity to add headers
    if (!pszAdditionalHeaders)
    {
        return E_POINTER;
    }

    *pszAdditionalHeaders = NULL;

    std::wstringstream extraHeaders;

    extraHeaders << L"Accept-Encoding: identity\r\n";

    if ( m_request->ranges.size() )
    {
        extraHeaders << L"Range: bytes=";
        for ( std::vector<BrowserStream::Range>::const_iterator it = m_request->ranges.begin(); it != m_request->ranges.end(); ++it )
        {
            extraHeaders << it->start << L"-" << (it->end - 1);
            if ( (it+1) != m_request->ranges.end() ) extraHeaders << L",";
        }
        extraHeaders << L"\r\n";
    }

    // This header is required when performing a POST operation
    if (BINDVERB_POST == m_dwAction && m_hDataToPost)
    {
        extraHeaders << L"Content-Type: application/x-www-form-urlencoded\r\n";
    }

    LPWSTR wszAdditionalHeaders = 
        (LPWSTR)CoTaskMemAlloc((extraHeaders.str().size()+1) *sizeof(WCHAR));
    if (!wszAdditionalHeaders)
    {
        return E_OUTOFMEMORY;
    }

    wcscpy(wszAdditionalHeaders, extraHeaders.str().c_str());
    *pszAdditionalHeaders = wszAdditionalHeaders;

    m_transactionStarted = true;

    return NOERROR;
}        

// ---------------------------------------------------------------------------
// %%Function: ActiveXBindStatusCallback::OnResponse
// ---------------------------------------------------------------------------
STDMETHODIMP ActiveXBindStatusCallback::OnResponse(/* [in] */ DWORD dwResponseCode,
        /* [unique][in] */ LPCWSTR szResponseHeaders,
        /* [unique][in] */ LPCWSTR szRequestHeaders,
        /* [out] */ LPWSTR __RPC_FAR *pszAdditionalRequestHeaders)
{
    if (!pszAdditionalRequestHeaders)
    {
        return E_POINTER;
    }

    *pszAdditionalRequestHeaders = NULL;

    if ( m_request->stream )
    {   
        m_request->stream->setHeaders( wstring_to_utf8( szResponseHeaders ) );

        bool requestedSeekable = m_request->stream->isSeekableRequested();

        std::string data;
        if ( GetInfo( HTTP_QUERY_CONTENT_LENGTH, data ) ) m_request->stream->setLength( atol( data.c_str() ) );     // nasty, should use a stringstream for conversion here
        if ( GetInfo( HTTP_QUERY_CONTENT_TYPE, data ) ) m_request->stream->setMimeType( data );
        if ( GetInfo( HTTP_QUERY_ACCEPT_RANGES, data ) ) m_request->stream->setSeekableByServer( data == "bytes" );

        bool ok = ( dwResponseCode >= 200 && dwResponseCode < 300 );
        if ( requestedSeekable && !m_request->stream->isSeekableByServer() ) ok = false;

        if ( ok )
        {
            if ( !m_request->stream->isOpen() ) m_request->stream->signalOpened();
            return NOERROR;
        }
        else
        {
            m_request->stream->signalFailedOpen();
            return E_ABORT;
        }       
    }

    return E_ABORT;
}


bool ActiveXBindStatusCallback::GetInfo(DWORD which, std::string& result)
{
    bool ok = false;

    DWORD bufferSize = 2048;
    boost::scoped_array<char> buffer(new char[bufferSize]);
    DWORD flags(0);

    CComPtr<IWinInetHttpInfo> httpInfo;
    if ( !FAILED(m_pbinding->QueryInterface( &httpInfo ) ) )
    {
        ok = ( S_OK == httpInfo->QueryInfo( which, &buffer[0], &bufferSize, &flags, 0 ) );
        if( ok )
            result = std::string( buffer.get(), bufferSize );
    }
    
    return ok;
}

bool ActiveXBindStatusCallback::close()
{
    HRESULT hr = m_pbinding->Abort();
    return (!FAILED( hr )) || ( hr == S_FALSE ) || ( hr == INET_E_RESULT_DISPATCHED );
}


ActiveXStreamRequest::ActiveXStreamRequest( ActiveXStreamPtr Stream ) : stream(Stream)
{
}

ActiveXStreamRequest::ActiveXStreamRequest( ActiveXStreamPtr Stream, const std::vector<BrowserStream::Range>& Ranges ) : stream(Stream), ranges(Ranges)
{
}

bool ActiveXStreamRequest::start()
{
    std::string url = stream->getUrl();
    std::wstring wideUrl( url.begin(), url.end() );

    if ( FAILED( ActiveXBindStatusCallback::Create( &bindStatusCallback, shared_from_this() )) ) return false;  
    if ( FAILED( CreateURLMonikerEx(0, wideUrl.c_str(), &FMoniker, URL_MK_UNIFORM) ) ) return false;
    if ( FAILED( CreateAsyncBindCtx(0, bindStatusCallback, 0, &FBindCtx) ) ) return false;
    if ( FAILED( IsValidURL(FBindCtx, wideUrl.c_str(), 0) ) ) return false;
    HRESULT hr = FMoniker->BindToStorage(FBindCtx, 0, IID_IStream, (void**)&fstream);

    if ( FAILED(hr) && (hr != MK_S_ASYNCHRONOUS) ) return false;

    return true;
}

bool ActiveXStreamRequest::stop()
{
    if ( !bindStatusCallback ) return true;
    bool retVal = bindStatusCallback->close();
    bindStatusCallback.Release();
    stream.reset();
    return retVal;
}

