/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Jan 23, 2010
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1

http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <cstdio>
#include <cassert>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include "JSObject.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "logging.h"
#include "../PluginCore/BrowserStreamManager.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "BrowserHost.h"
#include "SystemProxyDetector.h"

//////////////////////////////////////////
// This is used to keep async calls from
// crashing the browser on shutdown
//////////////////////////////////////////

namespace FB {
    struct _asyncCallData : boost::noncopyable {
        _asyncCallData(void (*func)(void*), void* userData, int id, AsyncCallManagerPtr mgr)
            : func(func), userData(userData), uniqId(id), called(false), mgr(mgr)
        {}
        void call();
        void (*func)(void *);
        void *userData;
        int uniqId;
        bool called;
        AsyncCallManagerWeakPtr mgr;
    };

    class AsyncCallManager : public boost::enable_shared_from_this<AsyncCallManager>, boost::noncopyable {
    public:
        int lastId;
        AsyncCallManager() : lastId(1) {}
        ~AsyncCallManager();

        boost::recursive_mutex m_mutex;
        void shutdown();

        _asyncCallData* makeCallback(void (*func)(void *), void * userData );
        void call( _asyncCallData* data );
        void remove( _asyncCallData* data );

        std::set<_asyncCallData*> DataList;
        std::set<_asyncCallData*> canceledDataList;
    };
}

volatile int FB::BrowserHost::InstanceCount(0);

FB::BrowserHost::BrowserHost()
    : _asyncManager(boost::make_shared<AsyncCallManager>()), m_threadId(boost::this_thread::get_id()),
      m_isShutDown(false), m_streamMgr(boost::make_shared<FB::BrowserStreamManager>()), m_htmlLogEnabled(true)
{
    ++InstanceCount;
}

FB::BrowserHost::~BrowserHost()
{
    --InstanceCount;
}

void FB::BrowserHost::shutdown()
{
    BOOST_FOREACH(FB::JSAPIPtr ptr, m_retainedObjects) {
        // Notify each JSAPI object that we're shutting down
        ptr->shutdown();
    }
    freeRetainedObjects();
    boost::upgrade_lock<boost::shared_mutex> _l(m_xtmutex);
    m_isShutDown = true;
    _asyncManager->shutdown();
    m_streamMgr.reset();
}

void FB::BrowserHost::htmlLog(const std::string& str)
{
    FBLOG_INFO("BrowserHost", "Logging to HTML: " << str);
    if (m_htmlLogEnabled) {
        try {
            this->ScheduleAsyncCall(&FB::BrowserHost::AsyncHtmlLog,
                new FB::AsyncLogRequest(shared_from_this(), str));
        } catch (const std::exception&) {
            // This fails during shutdown; ignore it
        }
    }
}

void FB::BrowserHost::AsyncHtmlLog(void *logReq)
{
    FB::AsyncLogRequest *req = (FB::AsyncLogRequest*)logReq;
    try {
        FB::DOM::WindowPtr window = req->m_host->getDOMWindow();

        if (window && window->getJSObject()->HasProperty("console")) {
            FB::JSObjectPtr obj = window->getProperty<FB::JSObjectPtr>("console");
            printf("Logging: %s\n", req->m_msg.c_str());
            if (obj)
                obj->Invoke("log", FB::variant_list_of(req->m_msg));
        }
    } catch (const std::exception &) {
        // printf("Exception: %s\n", e.what());
        // Fail silently; logging should not require success.
        FBLOG_TRACE("BrowserHost", "Logging to browser console failed");
        return;
    }
    delete req;
}

void FB::BrowserHost::evaluateJavaScript(const std::wstring &script)
{
    evaluateJavaScript(FB::wstring_to_utf8(script));
}

void FB::BrowserHost::initJS(const void* inst)
{
    assertMainThread();
    // Inject javascript helper function into the page; this is neccesary to help
    // with some browser compatibility issues.
    
    const char* javascriptMethod = 
        "window.__FB_CALL_%1% = "
        "function(delay, f, args, fname) {"
        "   if (arguments.length == 3)"
        "       return setTimeout(function() { f.apply(null, args); }, delay);"
        "   else"
        "       return setTimeout(function() { f[fname].apply(f, args); }, delay);"
        "};";
    
    // hash pointer to get a unique key for this plugin instance
    std::size_t inst_key = static_cast<std::size_t>(
        reinterpret_cast<std::ptrdiff_t>(inst));
    inst_key += (inst_key >> 3);

    unique_key = boost::lexical_cast<std::string>(inst_key);
    
    call_delegate = (boost::format("__FB_CALL_%1%") % inst_key).str();
    
    evaluateJavaScript((boost::format(javascriptMethod) % inst_key).str());
}

int FB::BrowserHost::delayedInvoke(const int delayms, const FB::JSObjectPtr& func,
                                    const FB::VariantList& args, const std::string& fname)
{
    assertMainThread();
    FB::JSObjectPtr delegate = getDelayedInvokeDelegate();
    if (!delegate)
        return -1;  // this is wrong (the return is meant to be the result of setTimeout)
    if (fname.empty())
        return delegate->Invoke("", FB::variant_list_of(delayms)(func)(args)).convert_cast<int>();
    else
        return delegate->Invoke("", FB::variant_list_of(delayms)(func)(args)(fname)).convert_cast<int>();
}

FB::JSObjectPtr FB::BrowserHost::getDelayedInvokeDelegate() {
    FB::DOM::WindowPtr win(getDOMWindow());
    if (win) {
        if (call_delegate.empty()) {
            initJS(this);
        }
        FB::JSObjectPtr delegate(win->getProperty<FB::JSObjectPtr>(call_delegate));
        if (!delegate) {
            // Sometimes the first try doesn't work; for some reason retrying generally does,
            // and from then on it works fine
            initJS(this);
            delegate = win->getProperty<FB::JSObjectPtr>(call_delegate);
        }
        return delegate;
    }
    return FB::JSObjectPtr();
}

FB::DOM::WindowPtr FB::BrowserHost::_createWindow(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::WindowPtr(new FB::DOM::Window(obj));
}

FB::DOM::DocumentPtr FB::BrowserHost::_createDocument(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::DocumentPtr(new FB::DOM::Document(obj));
}

FB::DOM::ElementPtr FB::BrowserHost::_createElement(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::ElementPtr(new FB::DOM::Element(obj));
}

FB::DOM::NodePtr FB::BrowserHost::_createNode(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::NodePtr(new FB::DOM::Node(obj));
}

void FB::BrowserHost::assertMainThread() const
{
#ifdef _DEBUG
    if (!isMainThread()) {
        FBLOG_FATAL("BrowserHost", "Trying to call something from the wrong thread!");
    }
    assert(isMainThread());
#endif
}

bool FB::BrowserHost::isMainThread() const
{
    return m_threadId == boost::this_thread::get_id();
}

void FB::BrowserHost::freeRetainedObjects() const
{
    boost::recursive_mutex::scoped_lock _l(m_jsapimutex);
    // This releases all stored shared_ptr objects that the browser is holding
    m_retainedObjects.clear();

    // This allows the browserhost to release any browser objects that were held by the retained
    // objects
    DoDeferredRelease();
}

void FB::BrowserHost::retainJSAPIPtr( const FB::JSAPIPtr& obj ) const
{
    boost::recursive_mutex::scoped_lock _l(m_jsapimutex);
    m_retainedObjects.push_back(obj);
}

void FB::BrowserHost::releaseJSAPIPtr( const FB::JSAPIPtr& obj ) const
{
    boost::recursive_mutex::scoped_lock _l(m_jsapimutex);
    std::list<FB::JSAPIPtr>::iterator it = std::find_if(m_retainedObjects.begin(), m_retainedObjects.end(), boost::lambda::_1 == obj);
    if (it != m_retainedObjects.end()) {
        m_retainedObjects.erase(it);
    }

    if (isMainThread())
        DoDeferredRelease();
}

void FB::_asyncCallData::call()
{
    if (func) {
        void (*f)(void *) = func;
        func = NULL;
        called = true;
        f(userData); 
    }
}


void FB::AsyncCallManager::call( _asyncCallData* data )
{
    {
        // Verify _asyncCallData is still in DataList. If not, the _asyncCallData has already been dealt with. 
        boost::recursive_mutex::scoped_lock _l(m_mutex);
        std::set<_asyncCallData*>::iterator fnd = DataList.find(data);
        if (DataList.end() != fnd)
            DataList.erase(fnd);
        else
            data = NULL;
    }
    if (data) {
        data->call();
        delete data;
    }
}

FB::_asyncCallData* FB::AsyncCallManager::makeCallback(void (*func)(void *), void * userData)
{
    boost::recursive_mutex::scoped_lock _l(m_mutex);
    _asyncCallData *data = new _asyncCallData(func, userData, ++lastId, shared_from_this());
    DataList.insert(data);
    return data;
}

void FB::AsyncCallManager::remove(_asyncCallData* data)
{
    boost::recursive_mutex::scoped_lock _l(m_mutex);
    DataList.erase(data);
}

void FB::AsyncCallManager::shutdown()
{
    boost::recursive_mutex::scoped_lock _l(m_mutex);
    // Store these so that they can be freed when the browserhost object is destroyed -- at that
    // point it's no longer possible for the browser to finish the async calls
    canceledDataList.insert(DataList.begin(), DataList.end());

    std::for_each(DataList.begin(), DataList.end(), boost::lambda::bind(&_asyncCallData::call, boost::lambda::_1));
    DataList.clear();
}

FB::AsyncCallManager::~AsyncCallManager()
{
    std::for_each(canceledDataList.begin(), canceledDataList.end(), boost::lambda::bind(boost::lambda::delete_ptr(), boost::lambda::_1));
}


void asyncCallWrapper(void *userData)
{
    // Verify AsyncCallManager still exists. If not, the _asyncCallData has already been dealt with.
    FB::_asyncCallData* data(static_cast<FB::_asyncCallData*>(userData));
    FB::AsyncCallManagerPtr ptr(data->mgr.lock());
    if (ptr) {
        ptr->call(data);
    }
}

bool FB::BrowserHost::ScheduleAsyncCall( void (*func)(void *), void *userData ) const
{
    if (isShutDown()) {
        return false;
    } else {
        _asyncCallData* data = _asyncManager->makeCallback(func, userData);
        bool result = _scheduleAsyncCall(&asyncCallWrapper, data);
        if (!result) {
            _asyncManager->remove(data);
        }
        return result;
    }
}

FB::BrowserStreamPtr FB::BrowserHost::createStream( const std::string& url,
    const PluginEventSinkPtr& callback, bool cache /*= true*/, bool seekable /*= false*/,
    size_t internalBufferSize /*= 128 * 1024 */ ) const
{
    assertMainThread();
    FB::BrowserStreamPtr ptr(_createStream(url, callback, cache, seekable, internalBufferSize));
    if (ptr) {
        m_streamMgr->retainStream(ptr);
    }
    return ptr;
}

FB::BrowserStreamPtr FB::BrowserHost::createPostStream( const std::string& url,
    const PluginEventSinkPtr& callback, const std::string& postdata, bool cache /*= true*/, 
    bool seekable /*= false*/, size_t internalBufferSize /*= 128 * 1024 */ ) const
{
    assertMainThread();
    FB::BrowserStreamPtr ptr(_createPostStream(url, callback, postdata, cache, seekable, internalBufferSize));
    if (ptr) {
        m_streamMgr->retainStream(ptr);
    }
    return ptr;
}

bool FB::BrowserHost::DetectProxySettings( std::map<std::string, std::string>& settingsMap, const std::string& url )
{
    return FB::SystemProxyDetector::get()->detectProxy(settingsMap, url);
}
