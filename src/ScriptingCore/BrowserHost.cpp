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
#include <memory>
#include <thread>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "JSObject.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "logging.h"
#include "../PluginCore/BrowserStreamManager.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "BrowserHost.h"
#include "BrowserStreamRequest.h"
#include "SystemProxyDetector.h"
#include "SimpleStreamHelper.h"

//////////////////////////////////////////
// This is used to keep async calls from
// crashing the browser on shutdown
//////////////////////////////////////////

const char* FB::BrowserHost::jsHelperTpl = "!function(){window.FireBreathHelperThingy={asyncCall:function(n,r,e,t){return t?setTimeout(function(){r[t].apply(r,e)},n):setTimeout(function(){r.apply(null,e)},n)},makeError:function(n){return new Error(n)}}}();";
const char* FB::BrowserHost::fbPromiseJS = "!function(){function e(e){var t=typeof e;return'function'===t||'object'===t&&!!e}function t(e,t){setTimeout(function(){e(t)},0)}function n(){function r(t){try{return e(t)?t.then:null}catch(n){return u.reject(n),!1}}var o,u=this,f=[],E=[],s=i.PEND;u.promise={then:function(e,r){function u(e,t){return function(n){try{if(c(e)){var r=e(n);a.resolve(r)}else a[t](n)}catch(o){a.reject(o)}}}var a=new n;return s===i.RESOLVE?t(function(){u(e,'resolve')(o)}):s===i.REJECT?t(function(){u(r,'reject')(o)}):(f.push(u(e,'resolve')),E.push(u(r,'reject'))),a.promise}},u.resolve=function(e,n){if(s!==i.RESOLVE&&s!==i.REJECT){if(e===u||e===u.promise)return u.reject(new TypeError);if(s===i.PEND||n){var E=r(e);if(E!==!1)if(c(E)){var a=!1,l=function(e){a||(a=!0,u.resolve(e,!0))},v=function(e){a||(a=!0,u.reject(e,!0))};try{s=i.PROMISE,E.call(e,l,v)}catch(p){a||(s=i.PEND,u.reject(p))}}else{o=e,s=i.RESOLVE;for(var h=0;h<f.length;++h)t(f[h],o)}}}},u.reject=function(e){if(s!==i.RESOLVE&&s!==i.REJECT){if(e===u||e===u.promise)return u.reject(new TypeError);o=e,s=i.REJECT;for(var n=0;n<E.length;++n)t(E[n],o)}}}function r(){return new n}var o=Object.prototype.toString,c=function(e){return'[object Function]'===o.call(e)};'function'!=typeof/./&&(c=function(e){return'function'==typeof e||!1});var i={PEND:1,PROMISE:2,RESOLVE:3,REJECT:4};this.FireBreathPromise=r}();";

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

    class AsyncCallManager : public std::enable_shared_from_this<AsyncCallManager>, boost::noncopyable {
    public:
        int lastId;
        AsyncCallManager() : lastId(1) {}
        ~AsyncCallManager();

        std::recursive_mutex m_mutex;
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
    : _asyncManager(std::make_shared<AsyncCallManager>()), m_threadId(std::this_thread::get_id()),
      m_isShutDown(false), m_streamMgr(std::make_shared<FB::BrowserStreamManager>()), m_htmlLogEnabled(true)
{
    ++InstanceCount;
}

FB::BrowserHost::~BrowserHost()
{
    --InstanceCount;
}

void FB::BrowserHost::shutdown()
{
    for (auto ptr : m_retainedObjects) {
        // Notify each JSAPI object that we're shutting down
        ptr->shutdown();
    }
    freeRetainedObjects();
    boost::upgrade_lock<boost::shared_mutex> _l(m_xtmutex);
    m_isShutDown = true;
    _asyncManager->shutdown();
    m_streamMgr.reset();
}

void FB::BrowserHost::htmlLog(std::string str)
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
            std::string logmsg = req->m_msg;
            window->getProperty<FB::JSObjectPtr>("console").done([logmsg](JSObjectPtr obj) {
                FBLOG_INFO("BrowserHost", "Logging: %s\n" << logmsg);
                if (obj)
                    obj->Invoke("log", FB::VariantList{logmsg});
            });
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
    return m_threadId == std::this_thread::get_id();
}

void FB::BrowserHost::freeRetainedObjects() const
{
    std::unique_lock<std::recursive_mutex> _l(m_jsapimutex);
    // This releases all stored shared_ptr objects that the browser is holding
    m_retainedObjects.clear();

    // This allows the browserhost to release any browser objects that were held by the retained
    // objects
    DoDeferredRelease();
}

void FB::BrowserHost::retainJSAPIPtr( FB::JSAPIPtr obj ) const
{
    std::unique_lock<std::recursive_mutex> _l(m_jsapimutex);
    m_retainedObjects.emplace_back(obj);
}

void FB::BrowserHost::releaseJSAPIPtr( FB::JSAPIPtr obj ) const
{
    std::unique_lock<std::recursive_mutex> _l(m_jsapimutex);
    auto it = std::find_if(m_retainedObjects.begin(), m_retainedObjects.end(), [&](decltype(m_retainedObjects)::value_type cur) { return cur == obj; });
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
        std::unique_lock<std::recursive_mutex> _l(m_mutex);
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
    std::unique_lock<std::recursive_mutex> _l(m_mutex);
    _asyncCallData *data = new _asyncCallData(func, userData, ++lastId, shared_from_this());
    DataList.insert(data);
    return data;
}

void FB::AsyncCallManager::remove(_asyncCallData* data)
{
    std::unique_lock<std::recursive_mutex> _l(m_mutex);
    DataList.erase(data);
}

void FB::AsyncCallManager::shutdown()
{
    std::unique_lock<std::recursive_mutex> _l(m_mutex);
    // Store these so that they can be freed when the browserhost object is destroyed -- at that
    // point it's no longer possible for the browser to finish the async calls
    canceledDataList.insert(DataList.begin(), DataList.end());

    for (auto i1 : DataList) { call(i1); }
    DataList.clear();
}

FB::AsyncCallManager::~AsyncCallManager()
{
    for (auto i1 : canceledDataList) { delete i1; }
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

FB::BrowserStreamPtr FB::BrowserHost::createStream( std::string url,
    const PluginEventSinkPtr& callback, bool cache /*= true*/, bool seekable /*= false*/,
    size_t internalBufferSize /*= 128 * 1024 */ ) const
{
    BrowserStreamRequest req(url, "GET");
    req.setEventSink(callback);
    req.setCacheable(cache);
    req.setSeekable(seekable);
    req.setBufferSize(internalBufferSize);
    return createStream(req);
}

FB::BrowserStreamPtr FB::BrowserHost::createStream( const BrowserStreamRequest& req, const bool enable_async ) const
{
    assertMainThread();
    if (enable_async && req.getCallback() && !req.getEventSink()) {
        // If a callback was provided, use SimpleStreamHelper to create it;
        // This will actually call back into this function with an event sink
        BrowserStreamRequest newReq(req);
        FB::SimpleStreamHelperPtr asyncPtr(SimpleStreamHelper::AsyncRequest(shared_from_this(), req));
        return asyncPtr->getStream();
    } else { // Create the stream with the EventSink
        FB::BrowserStreamPtr ptr(_createStream(req));
        if (ptr) {
            m_streamMgr->retainStream(ptr);
        }
        return ptr;
    }
}

FB::BrowserStreamPtr FB::BrowserHost::createPostStream( std::string url,
    const PluginEventSinkPtr& callback, std::string postdata, bool cache /*= true*/,
    bool seekable /*= false*/, size_t internalBufferSize /*= 128 * 1024 */ ) const
{
    BrowserStreamRequest req(url, "POST");
    req.setEventSink(callback);
    req.setCacheable(cache);
    req.setSeekable(seekable);
    req.setBufferSize(internalBufferSize);
    req.setPostData(postdata);
    return createStream(req);
}

FB::BrowserStreamPtr FB::BrowserHost::createUnsolicitedStream( const BrowserStreamRequest& req ) const
{
    assertMainThread();
    FB::BrowserStreamPtr ptr(_createUnsolicitedStream(req));
    if (ptr) {
        m_streamMgr->retainStream(ptr);
    }
    return ptr;
}

bool FB::BrowserHost::DetectProxySettings( std::map<std::string, std::string>& settingsMap, std::string url )
{
    return FB::SystemProxyDetector::get()->detectProxy(settingsMap, url);
}
