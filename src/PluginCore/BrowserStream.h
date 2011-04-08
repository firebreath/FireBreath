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

#pragma once
#ifndef H_FB_BROWSERSTREAM
#define H_FB_BROWSERSTREAM

#include <string>
#include "APITypes.h"
#include "PluginEventSource.h"
#include "PluginEventSink.h"
#include "PluginEvents/StreamEvents.h"

namespace FB {
    FB_FORWARD_PTR(BrowserStream);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  BrowserStream
    ///
    /// @brief  This is the abstract base class (interface class) for a browser stream.
    ///
    /// A BrowserStream is used to download and upload data via the browser host.
    /// The different hosts (NPAPI, ActiveX) derive from this class and provide an implementation.
    /// Instances are created by the factory method BrowserHost::createStream( ... parameters ... ).
    /// BrowserStreams send various events when the stream is created, destroyed or data in response to 
    /// a read request arrived.
    /// 
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class BrowserStream : public PluginEventSource
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  Range
        ///
        /// @brief  Specifies the range for a read range request (start to end) in bytes.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        struct Range
        {
            Range( size_t start, size_t end );

            size_t start, end;
        };

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn BrowserStream::BrowserStream( const std::string& url, bool cache, bool seekable, size_t internalBufferSize)
        ///
        /// @brief  Default constructor. Dont't use directly, use BrowserHost::createStream instead.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserStream( const std::string& url, bool cache, bool requestSeekable, size_t internalBufferSize);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual BrowserStream::~BrowserStream()
        ///
        /// @brief  Destructor.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BrowserStream();

        virtual BrowserStreamPtr shared_from_this() { return FB::ptr_cast<BrowserStream>(PluginEventSource::shared_from_this()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::readRange( size_t start, size_t end )
        ///
        /// @brief  Performs a read range request, returns the bytes in the range [start, end). 
        ///
        /// Only works if stream is seekable. Asynchronous, returns immediately. Listen for 
        /// StreamFailedOpenEvent, StreamOpenedEvent, StreamDataArrivedEvent to get any results.
        ///
        /// @author Matthias
        ///
        /// @return false on error, else true
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool    readRange( size_t start, size_t end );

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::readRanges( const std::vector<Range>& ranges ) = 0;
        ///
        /// @brief  Same functionality as BrowserStream::readRange( size_t start, size_t end ), except 
        ///         multiple ranges can be requested at once.
        ///
        /// @author Matthias
        ///
        /// @return false on error, else true
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool    readRanges( const std::vector<Range>& ranges ) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::write(const char* data, size_t dataLength, size_t& written) = 0;
        ///
        /// @brief  Writes data to the stream. 
        ///
        /// @param[in] data pointer to raw data to send
        /// @param[in] dataLength number of bytes provided by data
        /// @param[out] written the number of bytes written
        ///
        /// @author Matthias
        ///
        /// @return false on error, else true
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool    write(const char* data, size_t dataLength, size_t& written) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::close() = 0;
        ///
        /// @brief  Closes and destroys the stream.
        ///
        /// @author Matthias
        ///
        /// @return false on error, else true
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool    close() = 0;


    public:
        // property getters

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string BrowserStream::getUrl() const;
        ///
        /// @brief  Returns the url associated with this stream. This is the url supplied to 
        //          BrowserHost::createStream.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getUrl() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isSeekableByServer() const;
        ///
        /// @brief  Returns true if the stream is actually seekable (server says so).
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isSeekableByServer() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isSeekableRequested() const;
        ///
        /// @brief  Returns true if a stream was requested to be seekable. Not all servers support this.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isSeekableRequested() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isSeekable() const;
        ///
        /// @brief  Returns true if a stream was requested to be seekable and the server supports it.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isSeekable() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isCached() const;
        ///
        /// @brief  Returns true if the content of this stream should be cached (i.e. a physical file must
        ///         be created).
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isCached() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isCompleted() const;
        ///
        /// @brief  Returns true if download of this stream is completed (e.g. due to error or download finished).
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isCompleted() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool BrowserStream::isOpen() const;
        ///
        /// @brief  Returns true if the stream is open.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isOpen() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string BrowserStream::getMimeType() const;
        ///
        /// @brief  Returns the mime type of this stream, e.g. "text/plain".
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getMimeType() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::wstring BrowserStream::getCacheFilename() const;
        ///
        /// @brief  If this is a cached stream, returns the physical filename of the file in the browser's
        ///         cache.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::wstring getCacheFilename() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string BrowserStream::getHeaders() const;
        ///
        /// @brief  Returns the http headers.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getHeaders() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual size_t BrowserStream::getInternalBufferSize() const;
        ///
        /// @brief  Returns the size of the internal buffer used by this stream.
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t getInternalBufferSize() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual size_t BrowserStream::getLength() const;
        ///
        /// @brief  Returns the length of the stream in bytes (if server gives info like content-length).
        ///
        /// @author Matthias
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t getLength() const;

    protected:
        // property setters
        virtual void setUrl(const std::string& url);
        virtual void setSeekableByServer(bool seekable);
        virtual void setCached(bool cached);
        virtual void setCompleted(bool completed);
        virtual void setOpen(bool open);
        virtual void setMimeType(const std::string& mimeType);
        virtual void setCacheFilename(const std::wstring& cacheFilename);
        virtual void setHeaders(const std::string& headers);
        virtual void setInternalBufferSize(size_t internalBufferSize);
        virtual void setLength(size_t length);

    private:
        // properties
        std::string     url;
        bool            seekableRequested;
        bool            seekableByServer;
        bool            cached;
        size_t          internalBufferSize;
        std::wstring    cacheFilename;
        size_t          length;
        std::string     mimeType;
        bool            completed;
        bool            opened;
        std::string     headers;
    };
};

#endif

