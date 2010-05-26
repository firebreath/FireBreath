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

using namespace FB;

// browser stream
BrowserStream::BrowserStream( const std::string& Url, bool Cache, bool Seekable, size_t InternalBufferSize) : url(Url), seekable(Seekable), cached(Cache), internalBufferSize(InternalBufferSize), length(0), completed(false), opened(false)
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

bool BrowserStream::isSeekable() const
{
    return seekable;
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



void BrowserStream::setSeekable(bool Seekable)
{
    seekable = Seekable;
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


// default stream handler

bool DefaultBrowserStreamHandler::onStreamCreated(FB::StreamCreatedEvent *evt, FB::BrowserStream * Stream)
{
    stream = Stream;
    return false;
}

bool DefaultBrowserStreamHandler::onStreamDestroyed(FB::StreamDestroyedEvent *evt, FB::BrowserStream *)
{
    stream = 0;
    return false;
}

bool DefaultBrowserStreamHandler::onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *)
{
    // process data
    return false;
}

bool DefaultBrowserStreamHandler::onStreamFailedOpen(FB::StreamFailedOpenEvent *evt, FB::BrowserStream *)
{
    return false;
}

bool DefaultBrowserStreamHandler::onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *)
{
    return false;
}

bool DefaultBrowserStreamHandler::onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *)
{
     return false;
}