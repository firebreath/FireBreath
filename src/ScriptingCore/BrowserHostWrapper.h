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
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

namespace FB
{
    class BrowserStream;
    class PluginEventSink;
    class BrowserObjectAPI;
    namespace DOM {
        class Window;
        class Document;
        class Element;
        class Node;
        typedef boost::shared_ptr<Window> WindowPtr;
        typedef boost::shared_ptr<Document> DocumentPtr;
        typedef boost::shared_ptr<Element> ElementPtr;
        typedef boost::shared_ptr<Node> NodePtr;
    };

    struct AsyncLogRequest
    {
        AsyncLogRequest(BrowserHost host, const std::string& message) : m_host(host), m_msg(message) { }

        BrowserHost m_host;
        std::string m_msg;
    };

    class BrowserHostWrapper : public boost::enable_shared_from_this<BrowserHostWrapper>
    {
    public:
        BrowserHostWrapper() : m_threadId(boost::this_thread::get_id()) { }
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
        void assertMainThread();
        bool isMainThread();
        virtual DOM::DocumentPtr getDOMDocument() = 0;
        virtual DOM::WindowPtr getDOMWindow() = 0;
        virtual void evaluateJavaScript(const std::string &script) = 0;
        virtual void evaluateJavaScript(const std::wstring &script);
        virtual void htmlLog(const std::string& str);

public:
    // These methods are pseudo-public; they shouldn't be
    // called directly.  Call the ::create method on the 
    // DOM object you want
    virtual FB::DOM::WindowPtr _createWindow(const FB::JSObject& obj);
    virtual FB::DOM::DocumentPtr _createDocument(const FB::JSObject& obj);
    virtual FB::DOM::ElementPtr _createElement(const FB::JSObject& obj);
    virtual FB::DOM::NodePtr _createNode(const FB::JSObject& obj);

    protected:
        BrowserHost shared_ptr()
        {
            return shared_from_this();
        }

    protected:
        boost::thread::id m_threadId;
        std::string m_location;
    };
}

#endif
