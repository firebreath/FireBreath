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
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace FB
{
    FB_FORWARD_PTR(BrowserStream);
    FB_FORWARD_PTR(PluginEventSink);
    FB_FORWARD_PTR(JSObject);

    namespace DOM {
        FB_FORWARD_PTR(Window);
        FB_FORWARD_PTR(Document);
        FB_FORWARD_PTR(Element);
        FB_FORWARD_PTR(Node);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @struct AsyncLogRequest
    ///
    /// @brief  This class is used by BrowserHost for the BrowserHost::AsyncHtmlLog method
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct AsyncLogRequest
    {
        AsyncLogRequest(const boost::shared_ptr<BrowserHost>& host, const std::string& message) : m_host(host), m_msg(message) { }

        const boost::shared_ptr<BrowserHost> m_host;
        std::string m_msg;
    };

    FB_FORWARD_PTR(AsyncCallManager);
    FB_FORWARD_PTR(BrowserStreamManager);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  BrowserHost
    ///
    /// @brief  Browser host base class
    ///
    /// In actual use, this class will be used as either a NpapiBrowserHost or a ActiveXBrowserHost.
    /// This class provides APIs for making calls to the browser of any kind.  It is also used for
    /// making calls on the primary thread. There will always be exactly one BrowserHost object per
    /// plugin instance and it is unique to that instance
    /// @see NpapiBrowserHost
    /// @see ActiveXBrowserHost
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class BrowserHost : public boost::enable_shared_from_this<BrowserHost>, boost::noncopyable
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn BrowserHost() : m_threadId(boost::this_thread::get_id())
        ///
        /// @brief  Default constructor.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserHost();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~BrowserHost()
        ///
        /// @brief  Finaliser.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BrowserHost();

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool ScheduleAsyncCall(void (*func)(void *), void *userData) const
        ///
        /// @brief  Schedule asynchronous call to be executed on the main thread.
        ///
        /// This acts exactly the same way that NPN_PluginThreadAsyncCall would in NPAPI (and in most
        /// cases calls that function on NPAPI browsers).  It accepts a function of the type:
        /// @code
        ///     void funcName(void *userData)
        /// @endcode
        ///
        /// The provided function will be called with the userData on the main thread. If the
        /// plugin instance is shutting down this may fail and return false
        ///
        /// NOTE: This is a low level call; it is almost always better to use ScheduleOnMainThread
        ///
        /// @param  func     The function to call.
        /// @param  userData The userData to pass to the function.
        ///
        /// @returns bool true if the call was scheduled, false if it could not be
        ///
        /// @see ScheduleOnMainThread
        /// @see CallOnMainThread
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool ScheduleAsyncCall(void (*func)(void *), void *userData) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class Functor> typename Functor::result_type CallOnMainThread(Functor func)
        ///
        /// @brief  Execute a call on the main thread, wait for the result, and return it.
        ///
        /// With this template function calls can be made on the main thread like so:
        /// @code
        /// try {
        ///     int result = host->CallOnMainThread(boost::bind(&ObjectType::method, obj, arg1, arg2));
        /// } catch (const FB::script_error&) {
        ///     // The call will throw this exception if the browser is shutting down and it cannot
        ///     // be completed.
        /// }
        /// @endcode
        ///
        /// This is a synchronous cross-thread call, and as such may have a performance penalty.
        ///
        /// @param  func    The functor to execute on the main thread created with boost::bind
        ///
        /// @return The return value from the call
        /// @throws FB::script_error
        /// @see ScheduleOnMainThread
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class Functor>
        typename Functor::result_type CallOnMainThread(Functor func);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class C, class Functor> void ScheduleOnMainThread(const boost::shared_ptr<C>& obj, Functor func)
        ///
        /// @brief  Schedule a call to be executed on the main thread.
        ///
        /// With this template function a call can be scheduled to run asynchronously on the main thread
        /// like so:
        /// @code
        /// try {
        ///     boost::shared_ptr<ObjectType> obj(get_object_sharedptr());
        ///     host->ScheduleOnMainThread(obj, boost::bind(&ObjectType::method, obj, arg1, arg2));
        /// } catch (const FB::script_error&) {
        ///     // The call will throw this exception if the browser is shutting down and it cannot
        ///     // be completed.
        /// }
        /// @endcode
        ///
        /// Note that the first parameter should be a shared_ptr to the object the call is made on; this
        /// is then used to create a weak_ptr so that if the object goes away before the call is made
        /// the call can fail silently instead of crashing the browser.
        ///
        /// @param  obj     A boost::shared_ptr to the object that must exist when the call is made
        /// @param  func    The functor to execute on the main thread created with boost::bind
        /// @throws FB::script_error
        /// @see CallOnMainThread
        /// @see ScheduleAsyncCall
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class C, class Functor>
        void ScheduleOnMainThread(const boost::shared_ptr<C>& obj, Functor func);

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
        /// @fn virtual void *getContextID() const = 0
        ///
        /// @brief  Gets a unique identifier for this BrowserHost.  There are cases where you may need
        ///         multiple BrowserHosts in a single FireBreath plugin, primarily if you are embedding
        ///         some sort of web browser inside your plugin, and this is needed for that.
        ///
        /// @return null if it fails, else the context identifier.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void *getContextID() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual BrowserStreamPtr createStream(const std::string& url, const PluginEventSinkPtr& callback,
        /// bool cache = true, bool seekable = false, size_t internalBufferSize = 128 * 1024 ) const
        ///
        /// @brief  Creates a BrowserStream.
        ///
        /// @todo   Document this better
        ///
        /// @param  url                 URL of the document to request.
        /// @param  callback            PluginEventSink to send status updates to (usually your Plugin class
        ///                             derived from PluginCore)
        /// @param  cache               true to cache.
        /// @param  seekable            true if the Stream should be seekable.
        /// @param  internalBufferSize  Size of the internal buffer.
        ///
        /// @return null if it fails, else BrowserStream object
        /// @todo this should probably be a shared_ptr instead of a normal ptr
        /// @since 1.4a3 uses shared_ptrs instead of raw ptrs
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual BrowserStreamPtr createStream(const std::string& url, const PluginEventSinkPtr& callback,
                                            bool cache = true, bool seekable = false,
                                            size_t internalBufferSize = 128 * 1024 ) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual BrowserStreamPtr createPostStream(const std::string& url, const PluginEventSinkPtr& callback,
        /// const str::string postdata, bool cache = true, bool seekable = false, size_t internalBufferSize = 128 * 1024 ) const
        ///
        /// @brief  Creates a BrowserStream.
        ///
        /// @todo   Document this better
        ///
        /// @param  url                 URL of the document to request.
        /// @param  callback            PluginEventSink to send status updates to (usually your Plugin class
        ///                             derived from PluginCore)
        /// @param  postdata               data to post.
        /// @param  cache               true to cache.
        /// @param  seekable            true if the Stream should be seekable.
        /// @param  internalBufferSize  Size of the internal buffer.
        ///
        /// @return null if it fails, else BrowserStream object
        /// @todo this should probably be a shared_ptr instead of a normal ptr
        /// @since 1.4a3 uses shared_ptrs instead of raw ptrs
        virtual BrowserStreamPtr createPostStream(const std::string& url, const PluginEventSinkPtr& callback,
                                                const std::string& postdata,
                                                bool cache = true, bool seekable = false,
                                                size_t internalBufferSize = 128 * 1024 ) const;

        // Methods for accessing the DOM
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void assertMainThread() const
        ///
        /// @brief  When running in debug mode, asserts that the call is made on the main thread.
        /// @since 1.2.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void assertMainThread() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool isMainThread() const
        ///
        /// @brief  Query if this object is on the main thread.
        ///
        /// @return true if called on main thread, false if not.
        /// @since 1.3.0
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isMainThread() const;

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
        /// @fn virtual DOM::ElementPtr getDOMElement() = 0
        ///
        /// @brief  Gets a DOM::Element wrapper for the DOM/JS object tag that the plugin resides in.
        ///
        /// @return The dom element.
        /// @see DOM::Element
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DOM::ElementPtr getDOMElement() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Navigate(const std::string& url, const std::string& target)
        ///
        /// @brief  Instructs the browser to navigate to the specified url in the target window
        ///
        /// @param  url URL to navigate to
        /// @param  target Target window (for example, "_blank" will open a new window/tab)
        /// @since 1.6.0rc2
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void Navigate(const std::string& url, const std::string& target) {};

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void evaluateJavaScript(const std::string &script) = 0
        ///
        /// @brief  Evaluates arbitrary javascript; note that it does not return the result due to cross-
        ///         browser compatibility issues
        ///
        /// @param  script  The script to evaluate.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void evaluateJavaScript(const std::string &script) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void evaluateJavaScript(const std::wstring &script)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void evaluateJavaScript(const std::wstring &script);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn int delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
        ///                       const FB::VariantList& args, const std::string& fname = "");
        ///
        /// @brief  Executes the provided method object after a delay using window.setTimeout
        ///
        /// This is basically a wrapper for the Javascript setTimeout function that allows passing
        /// parameters to the function you want called after a delay. This is used behind the scenes
        /// by InvokeAsync with the delayms set to 0
        ///
        /// @param  delayms Delay time in milliseconds
        /// @param  func    Javascript object (or function)
        /// @param  args    Array of arguments to pass to the function
        /// @param  fname   If provided, the func.fname method will be called instead of invoking
        ///                 func as a function
        /// @return id returned by setTimeout
        ///
        /// @since 1.5.2
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        int delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
                          const FB::VariantList& args, const std::string& fname = "");
        FB::JSObjectPtr getDelayedInvokeDelegate();
        virtual void initJS(const void* inst);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void htmlLog(const std::string& str)
        ///
        /// @brief  Sends a log message to the containing web page using Console.log (firebug)
        ///
        /// @param  str The log message to send to the browser.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void htmlLog(const std::string& str);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void setEnableHtmlLog(const bool enabled = true)
        ///
        /// @brief  Enables or disables the htmlLog method
        ///
        /// @param  enabled true if html logging should be enabled
        ///
        /// @see htmlLog
        /// @since 1.5.2
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setEnableHtmlLog(const bool enabled = true) {
            m_htmlLogEnabled = enabled;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void shutdown()
        ///
        /// @brief  Notifies the browserhost object that the associated plugin object is shutting down
        ///
        /// This triggers events such as releasing all JSAPI objects that have been passed to the browser
        /// and disables cross-thread calls (since our context is going away if the plugin is shutting
        /// down)
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void shutdown();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn inline bool isShutDown() const
        ///
        /// @brief  returns true if the FB::BrowserHost::shutdown() method has been called on this object
        ///
        /// @return bool true if shutdown() has been called
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isShutDown() const { return m_isShutDown; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void freeRetainedObjects() const
        ///
        /// @brief  releases all JSAPI objects that have been passed to the browser
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void freeRetainedObjects() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void retainJSAPIPtr(const FB::JSAPIPtr& obj) const
        ///
        /// @brief  retains an instance of the JSAPI object until the plugin shuts down
        ///
        /// @param obj  JSAPIPtr object to retain
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void retainJSAPIPtr(const FB::JSAPIPtr& obj) const;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void releaseJSAPIPtr(const FB::JSAPIPtr& obj) const
        ///
        /// @brief  releases the specified JSAPI object to allow it to be invalidated and freed. This is
        ///         done automatically for all retained objects on shutdown
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void releaseJSAPIPtr(const FB::JSAPIPtr& obj) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void DoDeferredRelease() const
        ///
        /// @brief  Releases any browser-specific objects that were destroyed on a thread other than the
        ///         main thread.  Usually safe to just let FireBreath deal with this
        /// @since 1.4b3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void DoDeferredRelease() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool DetectProxySettings(std::map<std::string, std::string>& settingsMap, const std::string& url = "");
        ///
        /// @brief  Detects the proxy settings from the browser
        ///
        /// This will detect the proxy settings for a given URL from the web browser; in the case that
        /// The web browser itself is not being forthcoming with proxy settings (i.e. older versions of
        /// npapi browsers dont' support it) it will fall back to system proxy settings detection.
        ///
        /// settingsMap keys:
        ///    * type - the type of proxy, will be one of: socks, http, https, ftp
        ///    * hostname - hostname or address of the proxy
        ///    * port - port of the proxy
        ///
        /// @param settingsMap (out) populated with the proxy settings in a key => value format
        /// @param url the url for which you need proxy settings. Omit for the default proxy
        /// @since 1.6
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool DetectProxySettings(std::map<std::string, std::string>& settingsMap, const std::string& url = "");

    public:
        virtual FB::DOM::WindowPtr _createWindow(const FB::JSObjectPtr& obj) const;
        virtual FB::DOM::DocumentPtr _createDocument(const FB::JSObjectPtr& obj) const;
        virtual FB::DOM::ElementPtr _createElement(const FB::JSObjectPtr& obj) const;
        virtual FB::DOM::NodePtr _createNode(const FB::JSObjectPtr& obj) const;

    private:
        mutable AsyncCallManagerPtr _asyncManager;
        // Yes, this is supposed to be both private and pure virtual.
        virtual bool _scheduleAsyncCall(void (*func)(void *), void *userData) const = 0;
        virtual BrowserStreamPtr _createStream(const std::string& url, const PluginEventSinkPtr& callback,
                                            bool cache = true, bool seekable = false,
                                            size_t internalBufferSize = 128 * 1024 ) const = 0;
        virtual BrowserStreamPtr _createPostStream(const std::string& url, const PluginEventSinkPtr& callback,
                                            const std::string& postdata,
                                            bool cache = true, bool seekable = false,
                                            size_t internalBufferSize = 128 * 1024 ) const = 0;
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn BrowserHostPtr getInstanceCount()
        ///
        /// @brief  Returns the count of how many BrowserHost object instances are active
        ///
        /// @since 1.5
        /// @return int number of BrowserHost instances that have been created and not destroyed
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static int getInstanceCount() { return InstanceCount; }

    private:
        // Stores the thread_id for the thread the plugin was started on
        boost::thread::id m_threadId;
        // if true then the plugin this belongs to has shut down (or is shutting down)
        bool m_isShutDown;
        // Used to prevent race conditions with scheduling cross-thread calls during shutdown
        mutable boost::shared_mutex m_xtmutex;
        // Used to prevent race conditions with scheduling cross-thread calls during shutdown
        mutable boost::recursive_mutex m_jsapimutex;

        mutable std::list<FB::JSAPIPtr> m_retainedObjects;
        static volatile int InstanceCount;
        BrowserStreamManagerPtr m_streamMgr;

        // Indicates if html logging should be enabled (default true)
        bool m_htmlLogEnabled;

        std::string unique_key;
        std::string call_delegate;
    };


    // Due to interdependency issues, the implementation for ScheduleAsyncCall and CallOnMainThread are found in
    // CrossThreadCall.h
};

#include "CrossThreadCall.h"

#endif

