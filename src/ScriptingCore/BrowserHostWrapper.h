/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BROWSERHOSTWRAPPER
#define H_FB_BROWSERHOSTWRAPPER

#include "APITypes.h"
#include "AutoPtr.h"
#include <boost/enable_shared_from_this.hpp>

namespace FB
{
    class BrowserStream;
    class PluginEventSink;
    class BrowserObjectAPI;
    class JSAPI_DOMDocument;
    class JSAPI_DOMWindow;

    struct AsyncLogRequest
    {
        AsyncLogRequest(BrowserHost host, const std::string& message) : m_host(host), m_msg(message) { }

        BrowserHost m_host;
        std::string m_msg;
    };

    class BrowserHostWrapper : public boost::enable_shared_from_this<BrowserHostWrapper>
    {
    public:
        BrowserHostWrapper() { }
        virtual ~BrowserHostWrapper() { }

    public:
        virtual void ScheduleAsyncCall(void (*func)(void *), void *userData) = 0;

        static void AsyncHtmlLog(void *);

        virtual void *getContextID() = 0;

        virtual BrowserStream* createStream(const std::string& url, PluginEventSink* callback, 
                                            bool cache = true, bool seekable = false, 
                                            size_t internalBufferSize = 128 * 1024 ) = 0;
        
        // Methods for accessing the DOM
    public:
        virtual JSAPI_DOMDocument getDOMDocument() = 0;
        virtual JSAPI_DOMWindow getDOMWindow() = 0;
        virtual void evaluateJavaScript(const std::string &script) = 0;
        virtual void htmlLog(const std::string& str);

        BrowserHost shared_ptr()
        {
            return shared_from_this();
        }
    };
}

#endif
