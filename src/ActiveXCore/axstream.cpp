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

#include "axstream.h"
#include "axstream_impl.h"
#include "PluginEvents/StreamEvents.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;
using namespace FB::ActiveX;

ActiveXStream::ActiveXStream( const std::string& url, bool cache, bool seekableRequested, size_t internalBufferSize ) : FB::BrowserStream( url, cache, seekableRequested, internalBufferSize ), closing(false)
{
}

ActiveXStream::ActiveXStream( const std::string& url, bool cache, bool seekableRequested, size_t internalBufferSize, const std::string& verbdata ) : FB::BrowserStream( url, cache, seekableRequested, internalBufferSize ), closing(false), data(verbdata)
{
}

ActiveXStream::~ActiveXStream()
{
    close();
}

bool ActiveXStream::readRanges( const std::vector<Range>& ranges )
{
    if ( !isSeekable() || !isOpen() ) return false;
    if ( !ranges.size() ) return true;

    addRequest( ActiveXStreamRequest( FB::ptr_cast<ActiveXStream>(shared_from_this()), ranges ) );
    return true;
}

bool ActiveXStream::write(const char* data, size_t dataLength, size_t& written)
{
    return false;
}

bool ActiveXStream::close()
{
    if ( closing ) return true;         // prevent re-entrant calls
    closing = true;

    if ( !isOpen() ) return false;

    setOpen( false );

    bool result = true;
    std::set<ActiveXStreamRequestPtr> copiedRequests( requests );

    for ( std::set<ActiveXStreamRequestPtr>::const_iterator it = copiedRequests.begin(); it != copiedRequests.end(); ++it )
    {
        result &= (*it)->stop();
    }
    requests.clear();

    closing = false;
    return result;
}

bool ActiveXStream::init()
{
    if ( isSeekable() ) return true;                            // if seekable, wait for the user to pull data ...
    else return addRequest( ActiveXStreamRequest( FB::ptr_cast<ActiveXStream>(shared_from_this()) ) );     // ... otherwise start downloading the whole thing
}

bool ActiveXStream::addRequest( const ActiveXStreamRequest& Request )
{
    ActiveXStreamRequestPtr request( new ActiveXStreamRequest( Request ) );
    bool result = request->start();
    if ( result ) requests.insert( request );
    return result;
}


size_t ActiveXStream::signalDataArrived(void* buffer, size_t len, size_t offset)
{
    size_t effectiveLen = len; //min( getInternalBufferSize(), static_cast<size_t>(len) );
    if ( effectiveLen ) 
    {
        //memcpy( &internalBuffer[0], buffer, effectiveLen );
        
        float progress = 0;
        if ( getLength() )
        {
            progress = float( offset + len ) / float( getLength() ) * 100.f;
        }
        if ( isOpen() || (getCacheFilename() != L"") ) 
        {
            StreamDataArrivedEvent ev(this, buffer, effectiveLen, offset, progress);
            SendEvent( &ev );
        }
    }
    return effectiveLen;
}

void ActiveXStream::signalOpened()
{
    setOpen( true );
    StreamOpenedEvent ev(this);
    SendEvent( &ev );
}

void ActiveXStream::signalFailedOpen()
{
    StreamFailedOpenEvent ev(this);
    SendEvent( &ev );
}

void ActiveXStream::signalCompleted(bool success)
{
    setCompleted( true );

    if ( !isOpen() && !success )
    {
        signalFailedOpen();
    }

    close();

    StreamCompletedEvent ev(this, success);
    SendEvent( &ev );
}

void ActiveXStream::signalRequestCompleted(ActiveXStreamRequestPtr request, bool success)
{
    requests.erase( request );
    if ( !requests.size() )
    {
        signalCompleted( success );
        close();
    }
}
    
void ActiveXStream::signalCacheFilename(const std::wstring& cacheFilename)
{
    setCacheFilename( cacheFilename );
} 

std::string ActiveXStream::getVerbData() const
{
    return data;
}

#if 0
#ifdef USE_WINHTTP

// WinHTTP - Excerpt from MSDN docs on WinHttpOpen:
// INHTTP_ACCESS_TYPE_DEFAULT_PROXY does not inherit browser proxy settings. WinHTTP does not share any proxy settings with Internet Explorer. 
// The WinHTTP proxy configuration is set by one of these mechanisms:
//   - The proxycfg.exe utility on Windows XP and Windows Server 2003 or earlier.
//   - The netsh.exe utility on Windows Vista and Windows Server 2008 or later.
//   - WinHttpSetDefaultProxyConfiguration on all platforms.
//
// Checked this with Wireshark and indeed this method does not use any proxy, even if set in internet explorer.
//  Not good. Not sure about the automatic proxy stuff.
void UseWinHTTP(const WCHAR* URL)
{
  BOOL  bResults = FALSE;
    HINTERNET hSession = NULL,
              hConnect = NULL,
              hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(  L"DCWebClient/1.0", 
                             WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                             WINHTTP_NO_PROXY_NAME, 
                             WINHTTP_NO_PROXY_BYPASS, 0);

    URL_COMPONENTS comps;
    ZeroMemory(&comps, sizeof(comps));
    comps.dwStructSize = sizeof(comps);

    // Set required component lengths to non-zero 
    // so that they are cracked.
    comps.dwSchemeLength    = -1;
    comps.dwHostNameLength  = -1;
    comps.dwUrlPathLength   = -1;
    comps.dwExtraInfoLength = -1;
    comps.dwUrlPathLength   = -1;

    ::WinHttpCrackUrl( URL, 0, 0, &comps );

    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect( hSession, L"www.myurl.com",
                                   comps.nPort, 0);

    // Create an HTTP Request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest( hConnect, L"GET", 
                                       comps.lpszUrlPath, 
                                       NULL, WINHTTP_NO_REFERER, 
                                       WINHTTP_DEFAULT_ACCEPT_TYPES,
                                       0);

    const WCHAR headers[] = 
        L"Request-Range: bytes=2-5\r\n";
    // Send a Request.
    if (hRequest) 
        bResults = WinHttpSendRequest( hRequest,
                                       headers,
                                       0, WINHTTP_NO_REQUEST_DATA, 0, 
                                       0, 0);

    if (bResults)
        bResults = WinHttpReceiveResponse( hRequest, 0 );


    char buffer[1000];
    DWORD readBytes, availBytes;

    if (bResults)
        bResults = WinHttpQueryDataAvailable( hRequest, &availBytes );

    if (bResults && availBytes)
        bResults = WinHttpReadData( hRequest, buffer, availBytes, &readBytes );

    // PLACE ADDITIONAL CODE HERE.

    // Report any errors.
    if (!bResults)
        ShowMessage( 0, "Error %d has occurred." + toString(GetLastError()) );

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}

#endif

// WinInet - This one seems to work with Internet Explorer's proxies. It also uses internet explorer's default user agent and additional headers.
/* Wireshark example (with proxy enabled)
    GET /Resources.zip HTTP/1.0
    Accept: *//*
    Request-Range: bytes=2-5
    Accept-Encoding: gzip, deflate
    User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0)
    Host: www.myurl.com
    Pragma: no-cache
    Via: 1.1 myproxy.com:3128 (squid/2.7.STABLE7)
    X-Forwarded-For: 192.168.0.2
    Cache-Control: max-age=259200
    Connection: keep-alive

   Wireshark example (no proxy, direct connection):
    GET /Resources.zip HTTP/1.1
    Accept: *//*
    Request-Range: bytes=2-5
    Accept-Encoding: gzip, deflate
    User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0)
    Host: www.myurl.com
    Connection: Keep-Alive
    Cache-Control: no-cache
*/
void UseWinInet(const WCHAR* URL)
{
    HRESULT hr;

    IBindStatusCallback* ibscb(0);
    ::CBindStatusCallback::Create( &ibscb );
    /*HRESULT uoshr = URLOpenStream( 0, L"http://www.myurl.com/Resources.zip", 0, ibscb );
    ShowMessage( hwnd, "URLOpenStream: " + toString(uoshr) );*/
    IMoniker* FMoniker;
    CreateURLMonikerEx(0, URL, &FMoniker, URL_MK_UNIFORM );

    IBindCtx* FBindCtx;
    CreateAsyncBindCtx(0, ibscb, 0, &FBindCtx);

    hr = IsValidURL(FBindCtx, URL, 0);
    if ( hr != S_OK )
    {
        ShowMessage( 0, "Invalid url" );
    }
   
    IStream* fstream;
    hr = FMoniker->BindToStorage(FBindCtx, 0, IID_IStream, (void**)&fstream);

    if ( (hr != S_OK) && (hr != MK_S_ASYNCHRONOUS) )
    {
        ShowMessage( 0, "error" );
    }
}


// WinInet2 - This one seems to work with Internet Explorer's proxies. It does not use internet explorer's default user agent and only sends its own headers.
/* Wireshark example (with proxy enabled)
    GET /Resources.zip HTTP/1.0
    Request-Range: bytes=2-5
    User-Agent: DCWebPlayer/1.0
    Host: www.myurl.com
    Via: 1.1 myproxy.com:3128 (squid/2.7.STABLE7)
    X-Forwarded-For: 192.168.0.2
    Cache-Control: max-age=259200
    Connection: keep-alive

   Wireshark example (no proxy, direct connection):
    GET /Resources.zip HTTP/1.1
    Request-Range: bytes=2-5
    User-Agent: DCWebPlayer/1.0
    Host: www.myurl.com
    Connection: Keep-Alive
*/
void UseWinInet2(const WCHAR* URL)
{
    HINTERNET hSession = InternetOpen( L"DCWebPlayer/1.0", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0 /*INTERNET_FLAG_ASYNC*/ );
    if ( !hSession ) ShowMessage( 0, "could not open internet" );
    const WCHAR headers[] = L"Request-Range: bytes=2-5\r\n";
    HINTERNET handleurl = InternetOpenUrl( hSession, URL, headers, -1, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE, 0 );
    if ( !handleurl ) ShowMessage( 0, "could not open internet url" );

    char buf[1000];
    DWORD noBytes;
    InternetReadFile( handleurl, buf, 100, &noBytes );

    ShowMessage( 0, "WinInet2: "+ toString(noBytes) );
}
#endif

