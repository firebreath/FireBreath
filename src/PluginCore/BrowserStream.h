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

#ifndef H_FB_BROWSERSTREAM
#define H_FB_BROWSERSTREAM

#include <string>
#include "PluginEventSource.h"
#include "PluginEventSink.h"
#include "PluginEvents/StreamEvents.h"

namespace FB {

    /** 
     * BrowserStream
     *
     * This is the abstract base class for a browser stream.
     *
     * A BrowserStream is used to download and upload data via the browser host.
     *
     * The different hosts (NPAPI, ActiveX) derive from this class and provide an implementation.
     * Instances are created by the factory method BrowserHostWrapper::CreateStream( ... parameters ... ).
     * Sends various events when the stream is created, destroyed or data in response to a read
     *  request arrived.
     **/
    class BrowserStream : public PluginEventSource
    {
    public:
        /// Specifies the range for a read range request.
        struct Range
        {
            Range( size_t start, size_t end );

            size_t start, end;
        };

    public:
        /// Standard constructor.
        BrowserStream( const std::string& url, bool cache, bool seekable, size_t internalBufferSize);

        /// Destructor. The SendEvent() might be moved to Destroy() instead.
        virtual ~BrowserStream();

        /// Performs a read range request, returns the bytes in the range [start, end). Only works if stream is seekable.
        virtual bool    readRange( size_t start, size_t end );
        /// Same as function above, except multiple ranges can be requested at once.
        virtual bool    readRanges( const std::vector<Range>& ranges ) = 0;
        /// Writes data to the stream.
        virtual bool    write(const char* data, size_t dataLength, size_t& written) = 0;
        /// Destroys the stream.
        virtual bool    close() = 0;


    public:
        // property getters
        virtual std::string getUrl() const;
        virtual bool isSeekable() const;
        virtual bool isCached() const;
        virtual bool isCompleted() const;
        virtual bool isOpen() const;
        virtual std::string getMimeType() const;
        virtual std::wstring getCacheFilename() const;
        virtual std::string getHeaders() const;
        virtual size_t getInternalBufferSize() const;
        virtual size_t getLength() const;

    protected:
        // property setters
        virtual void setUrl(const std::string& url);
        virtual void setSeekable(bool seekable);
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
        bool            seekable;
        bool            cached;
        size_t          internalBufferSize;
        std::wstring    cacheFilename;
        size_t          length;
        std::string     mimeType;
        bool            completed;
        bool            opened;
        std::string     headers;
    };


    /// Simple implementation of a stream event handler, just derive from this class
    ///   and override the onStream* functions you'll need.
    class DefaultBrowserStreamHandler : public PluginEventSink
    {
    public:
        BEGIN_PLUGIN_EVENT_MAP()
            EVENTTYPE_CASE(FB::StreamCreatedEvent, onStreamCreated, FB::BrowserStream)
            EVENTTYPE_CASE(FB::StreamDestroyedEvent, onStreamDestroyed, FB::BrowserStream)
            EVENTTYPE_CASE(FB::StreamFailedOpenEvent, onStreamFailedOpen, FB::BrowserStream)
            EVENTTYPE_CASE(FB::StreamOpenedEvent, onStreamOpened, FB::BrowserStream)
            EVENTTYPE_CASE(FB::StreamDataArrivedEvent, onStreamDataArrived, FB::BrowserStream)
            EVENTTYPE_CASE(FB::StreamCompletedEvent, onStreamCompleted, FB::BrowserStream)
        END_PLUGIN_EVENT_MAP()

        virtual bool onStreamCreated(FB::StreamCreatedEvent *evt, FB::BrowserStream * Stream);
        virtual bool onStreamDestroyed(FB::StreamDestroyedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamFailedOpen(FB::StreamFailedOpenEvent *evt, FB::BrowserStream *);
        virtual bool onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *);

    protected:
        BrowserStream* stream;
    };

};

#endif