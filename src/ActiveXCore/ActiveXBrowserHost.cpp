/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "win_targetver.h"
#include "win_common.h"
#include <cstdint>
#include <memory>
#include "axstream.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "AsyncFunctionCall.h"
#include "Win/WinMessageWindow.h"
#include "AXDOM/Window.h"
#include "AXDOM/Document.h"
#include "AXDOM/Element.h"
#include "AXDOM/Node.h"

#include "ComVariantUtil.h"
#include "ActiveXFactoryDefinitions.h"
#include "ActiveXBrowserHost.h"
#include "BrowserStreamRequest.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include <boost/algorithm/string.hpp>

using boost::algorithm::replace_first;
using namespace FB;
using namespace FB::ActiveX;
using std::vector;
using std::string;

ActiveXBrowserHost::ActiveXBrowserHost(IWebBrowser2 *doc, IOleClientSite* site)
    : m_messageWin(new FB::WinMessageWindow())
{
    resume(doc, site);
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
    if (!isShutDown())
        shutdown();
}

bool ActiveXBrowserHost::_scheduleAsyncCall(void (*func)(void *), void *userData) const
{
    boost::shared_lock<boost::shared_mutex> _l(m_xtmutex);
    if (!isShutDown() && m_messageWin) {
        FBLOG_TRACE("ActiveXHost", "Scheduling async call for main thread");
        return ::PostMessage(m_messageWin->getHWND(), WM_ASYNCTHREADINVOKE, NULL,
            (LPARAM)new FB::AsyncFunctionCall(func, userData)) ? true : false;
    } else {
        return false;
    }
}

void *ActiveXBrowserHost::getContextID() const
{
    return (void*)this;
}

FB::DOM::WindowPtr ActiveXBrowserHost::_createWindow(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::WindowPtr(new AXDOM::Window(std::dynamic_pointer_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::DocumentPtr ActiveXBrowserHost::_createDocument(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::DocumentPtr(new AXDOM::Document(std::dynamic_pointer_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::ElementPtr ActiveXBrowserHost::_createElement(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::ElementPtr(new AXDOM::Element(std::dynamic_pointer_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::NodePtr ActiveXBrowserHost::_createNode(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::NodePtr(new AXDOM::Node(std::dynamic_pointer_cast<IDispatchAPI>(obj), m_webBrowser));
}

Promise<VariantList> ActiveXBrowserHost::GetArrayValues(FB::JSObjectPtr obj) {
	IDispatchAPIPtr ptr = std::dynamic_pointer_cast<IDispatchAPI>(obj);
	if (!ptr) {
		return VariantList();
	}

	uint32_t len = ptr->GetPropertySync("length").convert_cast<uint32_t>();
	VariantList out;
	for (size_t i{ 0 }; i < len; ++i) {
		out.emplace_back(ptr->GetPropertySync(i));
	}
	return out;
}

Promise<VariantMap> ActiveXBrowserHost::GetObjectValues(FB::JSObjectPtr obj) {
	VariantMap dst;
	IDispatchAPIPtr src = std::dynamic_pointer_cast<IDispatchAPI>(obj);
	if (!src) {
		return dst;
	}
	try {
		vector<string> fields;
		src->getMemberNames(fields);

		for (auto fname : fields) {
			dst[fname] = src->GetPropertySync(fname);
		}
	}
	catch (const FB::script_error& e) {
		throw e;
	}
	return dst;
}

IDispatchAPIPtr ActiveXBrowserHost::getPromiseObject() {
	IDispatchAPIPtr dfd;
	try {
		IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(m_window->getJSObject());
		dfd = std::dynamic_pointer_cast<IDispatchAPI>(win->InvokeSync("FireBreathPromise", FB::VariantList{}).cast<JSObjectPtr>());
	} catch (...) {
		// Didn't find it
		std::string jsStr{ BrowserHost::fbPromiseJS };
		this->evaluateJavaScript(jsStr);
		// This time if it throws an exception, let it throw
		IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(m_window->getJSObject());
		dfd = std::dynamic_pointer_cast<IDispatchAPI>(win->InvokeSync("FireBreathPromise", FB::VariantList{}).cast<JSObjectPtr>());
	}
	return dfd;
}

int FB::ActiveX::ActiveXBrowserHost::delayedInvoke(const int delayms, const FB::JSObjectPtr& func, const FB::VariantList& args, std::string fname /*= ""*/) {
    if (!m_htmlWin) { throw FB::script_error("Could not invoke, ActiveX control has been suspended"); }
    int32_t ctxId{ (int32_t)(getContextID()) };
    std::string name = std::string("_FB_HELPERS_") + std::to_string(ctxId);
    IDispatchAPIPtr helper;

    try {
        IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(m_window->getJSObject());
        helper = std::dynamic_pointer_cast<IDispatchAPI>(win->GetPropertySync(name).cast<JSObjectPtr>());
    } catch (const FB::bad_variant_cast&) {
        // Didn't find it
        std::string newjs{ BrowserHost::jsHelperTpl };
        replace_first(newjs, "FireBreathHelperThingy", name);
        this->evaluateJavaScript(newjs);
        // This time if it throws an exception, let it throw
        IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(m_window->getJSObject());
        helper = std::dynamic_pointer_cast<IDispatchAPI>(win->GetPropertySync(name).cast<JSObjectPtr>());
    }

    FB::variant v = helper->InvokeSync("asyncCall", FB::VariantList{ delayms, func, args, fname });

    return v.convert_cast<int>();
}

void ActiveXBrowserHost::initDOMObjects()
{
    // m_htmlWin/m_htmlDocDisp will be null after suspend()
    if (!m_window && m_htmlWin) {
        m_window = DOM::Window::create(IDispatchAPI::create(m_htmlWin, std::dynamic_pointer_cast<ActiveXBrowserHost>(shared_from_this())));
    }
    if (!m_document && m_htmlDocDisp) {
        m_document = DOM::Document::create(IDispatchAPI::create(m_htmlDocDisp, std::dynamic_pointer_cast<ActiveXBrowserHost>(shared_from_this())));
    }
}

FB::DOM::DocumentPtr ActiveXBrowserHost::getDOMDocument()
{
    initDOMObjects();
    return m_document;
}

FB::DOM::WindowPtr ActiveXBrowserHost::getDOMWindow()
{
    initDOMObjects();
    return m_window;
}

FB::DOM::ElementPtr ActiveXBrowserHost::getDOMElement()
{
    CComQIPtr<IOleControlSite> site(m_spClientSite);
    CComPtr<IDispatch> dispatch;
    site->GetExtendedControl(&dispatch);
    CComQIPtr<IHTMLElement2> htmlElement(dispatch);
    return DOM::Document::create(IDispatchAPI::create(htmlElement, std::dynamic_pointer_cast<ActiveXBrowserHost>(shared_from_this())));
}

void ActiveXBrowserHost::evaluateJavaScript(const std::string &script)
{
    if(!m_htmlWin) {
        throw FB::script_error("Can't execute JavaScript: Window is NULL");
    }

    CComVariant res;
    HRESULT hr = m_htmlWin->execScript(CComBSTR(script.c_str()),
                                       CComBSTR("javascript"), &res);
    if (SUCCEEDED(hr)) {
        /* Throw away returned VARIANT, this method always returns VT_EMPTY.
           http://msdn.microsoft.com/en-us/library/aa741364(VS.85).aspx */
        return;
    } else {
        throw FB::script_error("Error executing JavaScript code");
    }
}

void ActiveXBrowserHost::shutdown()
{
    {
        // First, make sure that no async calls are made while we're shutting down
        boost::upgrade_lock<boost::shared_mutex> _l(m_xtmutex);
        // Next, kill the message window so that none that have been made go through
        m_messageWin.reset();
    }
    ReleaseAllHeldObjects();

    // Finally, run the main browser shutdown, which will fire off any cross-thread
    // calls that somehow haven't made it through yet
    BrowserHost::shutdown();

    // Once that's done let's release any ActiveX resources that the browserhost
    // is holding
    suspend();
    assert(m_deferredObjects.empty());
}

void ActiveXBrowserHost::suspend()
{
    // release any ActiveX resources that the browserhost is holding
    m_webBrowser.Release();
    m_spClientSite.Release();
    m_htmlDocDisp.Release();
    m_htmlWin.Release();

    // These are created on demand, don't need to be restored
    m_window.reset();
    m_document.reset();

    DoDeferredRelease();
}
void ActiveXBrowserHost::resume(IWebBrowser2 *doc, IOleClientSite* clientSite)
{
    m_webBrowser = doc;
    m_spClientSite = clientSite;
    if (m_webBrowser && !m_htmlDocDisp) {
        m_webBrowser->get_Document(&m_htmlDocDisp);
        CComQIPtr<IHTMLDocument2> doc(m_htmlDocDisp);
        assert(doc);
        doc->get_parentWindow(&m_htmlWin);
        assert(m_htmlWin);
    }
}

FB::variant ActiveXBrowserHost::getVariant(const VARIANT *cVar)
{
    CComVariant converted;
    FB::variant retVal;

    switch(cVar->vt)
    {
    case VT_R4:
    case VT_R8:
    case VT_DECIMAL:
        converted.ChangeType(VT_R8, cVar);
        retVal = (double)converted.dblVal;
        break;

    case VT_I1:
    case VT_I2:
    case VT_I4:
    case VT_UI1:
    case VT_UI2:
    case VT_INT:
        converted.ChangeType(VT_I4, cVar);
        retVal = (long)converted.lVal;
        break;
    case VT_UI4:
    case VT_UINT:
        converted.ChangeType(VT_UI4, cVar);
        retVal = (unsigned long)converted.ulVal;
        break;

    case VT_I8:
        retVal = static_cast<int64_t>(cVar->llVal);
        break;
    case VT_UI8:
        retVal = static_cast<uint64_t>(cVar->ullVal);
    case VT_LPSTR:
    case VT_LPWSTR:
    case VT_BSTR:
    case VT_CLSID:
        {
            converted.ChangeType(VT_BSTR, cVar);
            std::wstring wStr(converted.bstrVal);

            // return it as a UTF8 std::string
            retVal = FB::wstring_to_utf8(wStr);
        }
        break;

    case VT_DISPATCH:
        retVal = FB::JSObjectPtr(IDispatchAPI::create(cVar->pdispVal, std::dynamic_pointer_cast<ActiveXBrowserHost>(shared_from_this())));
        break;

    case VT_ERROR:
    case VT_BOOL:
        converted.ChangeType(VT_BOOL, cVar);
        retVal = (converted.boolVal == VARIANT_TRUE) ? true : false;
        break;

    case VT_NULL:
        retVal = FB::FBNull();
        break;

    case VT_EMPTY:
    default:
        // retVal is already empty, leave it such
        break;
    }

    return retVal;
}

void ActiveXBrowserHost::getComVariant(VARIANT *dest, const FB::variant &var)
{
    CComVariant outVar;
    ::VariantInit(dest);

    const ComVariantBuilderMap& builderMap = getComVariantBuilderMap();
    const std::type_info& type = var.get_type();
    ComVariantBuilderMap::const_iterator it = builderMap.find(&type);

    if (it == builderMap.end()) {
        // unhandled type :(
        return;
    }

    outVar = (it->second)(std::dynamic_pointer_cast<ActiveXBrowserHost>(shared_from_this()), var);

    outVar.Detach(dest);
}

FB::BrowserStreamPtr FB::ActiveX::ActiveXBrowserHost::_createStream( const BrowserStreamRequest& req ) const
{
    assertMainThread();
    std::string url(req.uri.toString());
    ActiveXStreamPtr stream;
    if (req.method == "POST") {
        stream = std::make_shared<ActiveXStream>(url, req.cache, req.seekable, req.internalBufferSize, req.getPostData());
    } else {
        stream = std::make_shared<ActiveXStream>(url, req.cache, req.seekable, req.internalBufferSize);
    }
    if (req.getEventSink()) {
        stream->AttachObserver( req.getEventSink() );
    }

    if ( stream->init() )
    {
        StreamCreatedEvent ev(stream.get());
        stream->SendEvent( &ev );
        if ( req.seekable ) stream->signalOpened();
    }
    else
    {
        stream.reset();
    }
    return stream;
}

bool isExpired(std::pair<void*, FB::WeakIDispatchExRef> cur) {
    return cur.second.expired();
}

void ActiveXBrowserHost::DoDeferredRelease() const
{
    assertMainThread();
    IDispatchWRef deferred;
    while (m_deferredObjects.try_pop(deferred)) {
        if (deferred.expired())
            continue;
        IDispatchSRef ptr(deferred.lock());
        IDispatchRefList::iterator it(m_heldIDispatch.begin());
        IDispatchRefList::iterator end(m_heldIDispatch.end());
        while (it != end) {
            if (*it == ptr) {
                m_heldIDispatch.erase(it);
                break;
            } else ++it;
        }
        ptr->getPtr()->Release();
    }
    // Also remove any expired cached IDispatch WeakReferences
    IDispatchExRefMap::iterator iter = m_cachedIDispatch.begin();
    IDispatchExRefMap::iterator endIter = m_cachedIDispatch.end();
    while (iter != endIter) {
        if (isExpired(*iter))
            iter = m_cachedIDispatch.erase(iter);
        else
            ++iter;
    }
}


void FB::ActiveX::ActiveXBrowserHost::deferred_release( const IDispatchWRef& obj ) const
{
    m_deferredObjects.push(obj);
    if (isMainThread()) {
        DoDeferredRelease();
    }
}

IDispatchEx* FB::ActiveX::ActiveXBrowserHost::getJSAPIWrapper( const FB::JSAPIWeakPtr& api, bool autoRelease/* = false*/ )
{
    assertMainThread(); // This should only be called on the main thread
    using SharedIDispatchRef = std::shared_ptr<FB::ShareableReference<IDispatchEx> >;
    IDispatchEx* ret(nullptr);
    FB::JSAPIPtr ptr(api.lock());
    if (!ptr)
        return getFactoryInstance()->createCOMJSObject(shared_from_this(), api, autoRelease);

    IDispatchExRefMap::iterator fnd = m_cachedIDispatch.find(ptr.get());
    if (fnd != m_cachedIDispatch.end()) {
        SharedIDispatchRef ref(fnd->second.lock());
        if (ref) {
            // Fortunately this doesn't have to be threadsafe since this method only gets called
            // from the main thread and the browser access happens on that thread as well!
            ret = ref->getPtr();
            ret->AddRef();
        } else {
            m_cachedIDispatch.erase(fnd);
        }
    }
    if (!ret) {
        ret = getFactoryInstance()->createCOMJSObject(shared_from_this(), api, autoRelease);
        m_cachedIDispatch[ptr.get()] = _getWeakRefFromCOMJSWrapper(ret);
    }
    return ret;
}

FB::ActiveX::IDispatchWRef FB::ActiveX::ActiveXBrowserHost::getIDispatchRef( IDispatch* obj )
{
    obj->AddRef();
    auto ref = std::make_shared<FB::ShareableReference<IDispatch> >(obj);
    m_heldIDispatch.emplace_back(ref);
    return ref;
}

void FB::ActiveX::ActiveXBrowserHost::ReleaseAllHeldObjects()
{
    for (IDispatchRefList::iterator it(m_heldIDispatch.begin()); it != m_heldIDispatch.end(); ++it) {
        (*it)->getPtr()->Release();
    }
    m_heldIDispatch.clear();
}

void FB::ActiveX::ActiveXBrowserHost::Navigate( std::string url, std::string target )
{
    CComBSTR destURL(FB::utf8_to_wstring(url).c_str());
    CComVariant targetWin(FB::utf8_to_wstring(target).c_str());

    CComVariant vEmpty;

    HRESULT hr = m_webBrowser->Navigate(destURL, &vEmpty, &targetWin, &vEmpty, &vEmpty);
    assert(SUCCEEDED(hr));
}
