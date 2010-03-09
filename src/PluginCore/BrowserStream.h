/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
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
            Range( size_t Start, size_t End ) : start(Start), end(End)
            {}

            size_t start, end;
        };

    public:
        /// Standard constructor.
        BrowserStream( const std::string& Url, bool Cache, bool Seekable, size_t InternalBufferSize) : url(Url), seekable(Seekable), cached(Cache), internalBufferSize(InternalBufferSize), length(0), completed(false), opened(false)
        {}

        /// Destructor. The SendEvent() might be moved to Destroy() instead.
        virtual ~BrowserStream()
        {
            StreamDestroyedEvent ev(this);
            SendEvent( &ev );
        }

        /// Performs a read range request, returns the bytes in the range [start, end). Only works if stream is seekable.
        virtual bool	readRange( size_t start, size_t end )
        {
            return readRanges( std::vector<Range>( 1, Range(start, end) ) );
        }
        /// Same as function above, except multiple ranges can be requested at once.
        virtual bool	readRanges( const std::vector<Range>& ranges ) = 0;
        /// Writes data to the stream.
        virtual bool	write(const char* data, size_t dataLength, size_t& written) = 0;
        /// Destroys the stream.
        virtual bool	close() = 0;


        virtual
        std::string getUrl()
        {
            return url;
        }

        virtual
        bool isSeekable()
        {
            return seekable;
        }

        virtual
        bool isCached()
        {
            return cached;
        }

        virtual
        size_t getInternalBufferSize()
        {
            return internalBufferSize;
        }

        virtual
        std::wstring getCacheFilename()
        {
            return cacheFilename;
        }

        virtual
        size_t getLength()
        {
            return length;
        }

        virtual
        std::string getMimeType()
        {
            return mimeType;
        }

        virtual
        bool isCompleted()
        {
            return completed;
        }

        virtual
        bool isOpen()
        {
            return opened;
        }

        virtual
        std::string getHeaders()
        {
            return headers;
        }

    protected:
        std::string		url;
        bool			seekable;
        bool			cached;
        size_t			internalBufferSize;
        std::wstring	cacheFilename;
        size_t			length;
        std::string		mimeType;
        bool			completed;
        bool			opened;
        std::string		headers;
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

        virtual bool onStreamCreated(FB::StreamCreatedEvent *evt, FB::BrowserStream * Stream)
        {
            stream = Stream;
            return false;
        }
        virtual bool onStreamDestroyed(FB::StreamDestroyedEvent *evt, FB::BrowserStream *)
        {
            stream = 0;
            return false;
        }
        virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *)
        {
            // process data
            return false;
        }
        virtual bool onStreamFailedOpen(FB::StreamFailedOpenEvent *evt, FB::BrowserStream *)
        {
            return false;
        }
        virtual bool onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *)
        {
            return false;
        }
        virtual bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *)
        {
            return false;
        }

    protected:
        BrowserStream* stream;
    };

};

#endif