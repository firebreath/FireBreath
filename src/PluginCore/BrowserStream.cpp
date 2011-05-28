/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "BrowserStream.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;

// browser stream
BrowserStream::BrowserStream(const std::string& Url, bool Cache, bool SeekableRequested, size_t InternalBufferSize) 
  : url(Url)
  , seekableRequested(SeekableRequested)
  , seekableByServer(false)
  , cached(Cache)
  , internalBufferSize(InternalBufferSize)
  , length(0)
  , completed(false)
  , opened(false)
{
}

BrowserStream::~BrowserStream()
{
    StreamDestroyedEvent ev(this);
    SendEvent( &ev );
}

bool BrowserStream::readRange( size_t start, size_t end )
{
    return readRanges( std::vector<Range>( 1, Range(start, end) ) );
}

std::string BrowserStream::getUrl() const
{
    return url;
}

bool BrowserStream::isSeekableRequested() const
{
    return seekableRequested;
}

bool BrowserStream::isSeekableByServer() const
{
    return seekableByServer;
}

bool BrowserStream::isSeekable() const
{
    return isSeekableRequested() && isSeekableByServer();
}

bool BrowserStream::isCached() const
{
    return cached;
}

bool BrowserStream::isOpen() const
{
    return opened;
}

bool BrowserStream::isCompleted() const
{
    return completed;
}

std::string BrowserStream::getMimeType() const
{
    return mimeType;
}

std::wstring BrowserStream::getCacheFilename() const
{
    return cacheFilename;
}

std::string BrowserStream::getHeaders() const
{
    return headers;
}

size_t BrowserStream::getLength() const
{
    return length;
}

size_t BrowserStream::getInternalBufferSize() const
{
    return internalBufferSize;
}



void BrowserStream::setSeekableByServer(bool SeekableByServer)
{
    seekableByServer = SeekableByServer;
}

void BrowserStream::setCached(bool Cached)
{
    cached = Cached;
}

void BrowserStream::setOpen(bool Opened)
{
    opened = Opened;
}

void BrowserStream::setCompleted(bool Completed)
{
    completed = Completed;
}

void BrowserStream::setUrl(const std::string& Url)
{
    url = Url;
}

void BrowserStream::setHeaders(const std::string& Headers)
{
    headers = Headers;
}

void BrowserStream::setMimeType(const std::string& MimeType)
{
    mimeType = MimeType;
}

void BrowserStream::setCacheFilename(const std::wstring& CacheFilename)
{
    cacheFilename = CacheFilename;
}

void BrowserStream::setLength(size_t Length)
{
    length = Length;
}

void BrowserStream::setInternalBufferSize(size_t InternalBufferSize)
{
    internalBufferSize = InternalBufferSize;
}

// range

BrowserStream::Range::Range(size_t Start, size_t End) : start(Start), end(End)
{
}

