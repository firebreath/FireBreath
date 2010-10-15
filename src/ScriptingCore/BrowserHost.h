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
/// @brief These are convenience objects used to access and manipulate DOM objects.  They should
/// 	   never be created directly; instead, get the Window object or the Document object
/// 	   from the BrowserHost or create it with DOM::Element::create(obj) (or similar)
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
        /// @fn virtual void ScheduleAsyncCall(void (*func)(void *), void *userData) = 0
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class Functor> typename Functor::result_type CallOnMainThread(Functor func)
        ///
        /// @brief  Execute a call on the main thread, wait for the result, and return it.
        ///
        /// With this template function calls can be made on the main thread like so:
        /// @code
        /// 	 int result = host->CallOnMainThread(boost::bind(&ObjectType::method, obj, arg1, arg2));
        /// @endcode
        /// 
        /// This is a synchronous cross-thread call, and as such may have a performance penalty
        /// 
        /// @param  func    The functor to execute on the main thread created with boost::bind
        ///
        /// @return The return value from the call
        /// @see ScheduleOnMainThread
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class Functor>
        typename Functor::result_type CallOnMainThread(Functor func);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class C, class Functor> void ScheduleOnMainThread(boost::shared_ptr<C> obj, Functor func)
        ///
        /// @brief  Schedule a call to be executed on the main thread. 
        ///
        /// With this template function a call can be scheduled to run asynchronously on the main thread
        /// like so:
        /// @code
        /// 	 boost::shared_ptr<ObjectType> obj(get_object_sharedptr());
        ///      host->ScheduleOnMainThread(obj, boost::bind(&ObjectType::method, obj, arg1, arg2));
        /// @endcode
        /// 		
        /// Note that the first parameter should be a shared_ptr to the object the call is made on; this
        /// is then used to create a weak_ptr so that if the object goes away before the call is made
        /// the call can fail silently instead of crashing the browser.
        ///
        /// @param  obj     A boost::shared_ptr to the object that must exist when the call is made
        /// @param  func    The functor to execute on the main thread created with boost::bind
        /// @see CallOnMainThread
        /// @see ScheduleAsyncCall
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class C, class Functor>
        void ScheduleOnMainThread(boost::shared_ptr<C> obj, Functor func);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void AsyncHtmlLog(void *)
        ///
        /// @brief  Don't call this; it is a helper function used by htmlLog
        ///
        /// This method is as the callback on a call to SchedulAsyncCall by htmlLog.
        ///
        /// @param data an AsyncLogRequest object in a void*
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void AsyncHtmlLog(void *data);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void *getContextID() = 0
        ///
        /// @brief  Gets a unique identifier for this BrowserHost.  There are cases where you may need
        /// 		multiple BrowserHosts in a single FireBreath plugin, primarily if you are embedding
        /// 		some sort of web browser inside your plugin, and this is needed for that.
        ///
        /// @return null if it fails, else the context identifier. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void *getContextID() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual BrowserStream* createStream(const std::string& url, PluginEventSink* callback,
        /// bool cache = true, bool seekable = false, size_t internalBufferSize = 128 * 1024 ) = 0
        ///
        /// @brief  Creates a BrowserStream. 
        ///
        /// @todo   Document this better
        ///
        /// @param  url                 URL of the document to request. 
        /// @param  callback            PluginEventSink to send status updates to (usually your Plugin class
        /// 							derived from PluginCore)
        /// @param  cache               true to cache. 
        /// @param  seekable            true if the Stream should be seekable. 
        /// @param  internalBufferSize  Size of the internal buffer. 
        ///
        /// @return null if it fails, else BrowserStream object
        /// @todo this should probably be a shared_ptr instead of a normal ptr
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual BrowserStream* createStream(const std::string& url, PluginEventSink* callback, 
                                            bool cache = true, bool seekable = false, 
                                            size_t internalBufferSize = 128 * 1024 ) = 0;
        
        // Methods for accessing the DOM
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void assertMainThread()
        ///
        /// @brief  When running in debug mode, asserts that the call is made on the main thread. 
        /// @since 1.2.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void assertMainThread();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool isMainThread()
        ///
        /// @brief  Query if this object is on the main thread. 
        ///
        /// @return true if called on main thread, false if not. 
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isMainThread();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual DOM::DocumentPtr getDOMDocument() = 0
        ///
        /// @brief  Gets a DOM::Document wrapper for the document object that the plugin resides in. 
        ///
        /// @return The dom document. 
        /// @see DOM::Document
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DOM::DocumentPtr getDOMDocument() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual DOM::WindowPtr getDOMWindow() = 0
        ///
        /// @brief  Gets a DOM::Window wrapper for the DOM/JS window object that the plugin resides in.
        ///
        /// @return The dom window. 
        /// @see DOM::Window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DOM::WindowPtr getDOMWindow() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void evaluateJavaScript(const std::string &script) = 0
        ///
        /// @brief  Evaluates arbitrary javascript; note that it does not return the result due to cross-
        /// 		browser compatibility issues
        ///
        /// @param  script  The script to evaluate. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void evaluateJavaScript(const std::string &script) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void evaluateJavaScript(const std::wstring &script)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void evaluateJavaScript(const std::wstring &script);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void htmlLog(const std::string& str)
        ///
        /// @brief  Sends a log message to the containing web page using Console.log (firebug)
        ///
        /// @param  str The log message to send to the browser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void htmlLog(const std::string& str);

public:
    virtual FB::DOM::WindowPtr _createWindow(const FB::JSObjectPtr& obj);
    virtual FB::DOM::DocumentPtr _createDocument(const FB::JSObjectPtr& obj);
    virtual FB::DOM::ElementPtr _createElement(const FB::JSObjectPtr& obj);
    virtual FB::DOM::NodePtr _createNode(const FB::JSObjectPtr& obj);

    protected:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn BrowserHostPtr shared_ptr()
        ///
        /// @brief  Provides a BrowserHostPtr shared_ptr for use in situations where you would normally
        /// 		provide the "this" pointer.
        ///
        /// @return BrowerHostPtr for "this" pointer
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserHostPtr shared_ptr()
        {
            return shared_from_this();
        }

    protected:
        boost::thread::id m_threadId;   // Stores the thread_id for the thread the plugin was started on
    };

    
    // Due to interdependency issues, the implementation for ScheduleAsyncCall and CallOnMainThread are found in
    // CrossThreadCall.h
};

#include "CrossThreadCall.h"

#endif
