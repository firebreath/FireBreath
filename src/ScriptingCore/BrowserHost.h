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

#pragma once
#ifndef H_FB_BROWSERHOSTWRAPPER
#define H_FB_BROWSERHOSTWRAPPER

#include "APITypes.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

namespace FB
{
    class BrowserStream;
    class PluginEventSink;
    class JSObject;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @namespace  DOM {
///
/// @brief  .
////////////////////////////////////////////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @struct AsyncLogRequest
    ///
    /// @brief  This class is used by BrowserHost for the BrowserHost::AsyncHtmlLog method
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct AsyncLogRequest
    {
        AsyncLogRequest(BrowserHostPtr host, const std::string& message) : m_host(host), m_msg(message) { }

        BrowserHostPtr m_host;
        std::string m_msg;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  BrowserHost
    ///
    /// @brief  Browser host base class
    ///
    /// In actual use, this class will be used as either a NpapiBrowserHost or a ActiveXBrowserHost.
    /// This class provides APIs for making calls to the browser of any kind.  It is also used for
    /// making calls on the primary thread.
    /// @see NpapiBrowserHost
    /// @see ActiveXBrowserHost
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class BrowserHost : public boost::enable_shared_from_this<BrowserHost>
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn BrowserHost() : m_threadId(boost::this_thread::get_id())
        ///
        /// @brief  Default constructor. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserHost() : m_threadId(boost::this_thread::get_id()) { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~BrowserHost()
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BrowserHost() { }

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void :::ScheduleAsyncCall(void (*func)(void *), void *userData) = 0
        ///
        /// @brief  Schedule asynchronous call to be executed on the main thread. 
        ///
        /// This acts exactly the same way that NPN_PluginThreadAsyncCall would in NPAPI (and in most
        /// cases calls that function on NPAPI browsers).  It accepts a function of the type:
        /// @code
        ///     void funcName(void *userData)
        /// @endcode
        ///
        /// The provided function will be called with the userData on the main thread.
        /// 
        /// NOTE: This is a low level call; it is almost always easier to use ScheduleOnMainThread
        /// 
        /// @param  func     The function to call. 
        /// @param  userData The userData to pass to the function.
        /// 				 
        /// @see ScheduleOnMainThread
        /// @see CallOnMainThread
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void ScheduleAsyncCall(void (*func)(void *), void *userData) = 0;

        template<class Functor>
        typename Functor::result_type CallOnMainThread(Functor func);

        template<class C, class Functor>
        void ScheduleOnMainThread(boost::shared_ptr<C> obj, Functor func);

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
    virtual FB::DOM::WindowPtr _createWindow(const FB::JSObjectPtr& obj);
    virtual FB::DOM::DocumentPtr _createDocument(const FB::JSObjectPtr& obj);
    virtual FB::DOM::ElementPtr _createElement(const FB::JSObjectPtr& obj);
    virtual FB::DOM::NodePtr _createNode(const FB::JSObjectPtr& obj);

    protected:
        BrowserHostPtr shared_ptr()
        {
            return shared_from_this();
        }

    protected:
        boost::thread::id m_threadId;
        std::string m_location;
    };

    
    // Due to interdependency issues, the implementation for ScheduleAsyncCall and CallOnMainThread are found in
    // CrossThreadCall.h
};

#include "CrossThreadCall.h"

#endif
