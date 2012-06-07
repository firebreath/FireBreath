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

#include <boost/assign.hpp>
#include <boost/smart_ptr/make_shared.hpp>
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
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;
using namespace FB::ActiveX;

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
    return FB::DOM::WindowPtr(new AXDOM::Window(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::DocumentPtr ActiveXBrowserHost::_createDocument(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::DocumentPtr(new AXDOM::Document(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::ElementPtr ActiveXBrowserHost::_createElement(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::ElementPtr(new AXDOM::Element(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::NodePtr ActiveXBrowserHost::_createNode(const FB::JSObjectPtr& obj) const
{
    return FB::DOM::NodePtr(new AXDOM::Node(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

void ActiveXBrowserHost::initDOMObjects()
{
    // m_htmlWin/m_htmlDocDisp will be null after suspend()
    if (!m_window && m_htmlWin) {
        m_window = DOM::Window::create(IDispatchAPI::create(m_htmlWin, ptr_cast<ActiveXBrowserHost>(shared_from_this())));
    }
    if (!m_document && m_htmlDocDisp) {
        m_document = DOM::Document::create(IDispatchAPI::create(m_htmlDocDisp, ptr_cast<ActiveXBrowserHost>(shared_from_this())));
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
    return DOM::Document::create(IDispatchAPI::create(htmlElement, ptr_cast<ActiveXBrowserHost>(shared_from_this())));
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
        retVal = static_cast<boost::int64_t>(cVar->llVal);
        break;
    case VT_UI8:
        retVal = static_cast<boost::uint64_t>(cVar->ullVal);
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
        retVal = FB::JSObjectPtr(IDispatchAPI::create(cVar->pdispVal, ptr_cast<ActiveXBrowserHost>(shared_from_this()))); 
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
    
    outVar = (it->second)(FB::ptr_cast<ActiveXBrowserHost>(shared_from_this()), var);

    outVar.Detach(dest);
}

FB::BrowserStreamPtr ActiveXBrowserHost::_createStream(const std::string& url, const FB::PluginEventSinkPtr& callback, 
                                    bool cache, bool seekable, size_t internalBufferSize ) const
{
    assertMainThread();
    ActiveXStreamPtr stream(boost::make_shared<ActiveXStream>(url, cache, seekable, internalBufferSize));
    stream->AttachObserver( callback );

    if ( stream->init() )
    {
        StreamCreatedEvent ev(stream.get());
        stream->SendEvent( &ev );
        if ( seekable ) stream->signalOpened();
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

FB::BrowserStreamPtr ActiveXBrowserHost::_createPostStream(const std::string& url, const FB::PluginEventSinkPtr& callback, 
                                    const std::string& postdata, bool cache, bool seekable, size_t internalBufferSize ) const
{
    assertMainThread();
    ActiveXStreamPtr stream(boost::make_shared<ActiveXStream>(url, cache, seekable, internalBufferSize, postdata));
    stream->AttachObserver( callback );

    if ( stream->init() )
    {
        StreamCreatedEvent ev(stream.get());
        stream->SendEvent( &ev );
        if ( seekable ) stream->signalOpened();
    }
    else
    {
        stream.reset();
    }
    return stream;
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
    typedef boost::shared_ptr<FB::ShareableReference<IDispatchEx> > SharedIDispatchRef;
    IDispatchEx* ret(NULL);
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
    IDispatchSRef ref(boost::make_shared<FB::ShareableReference<IDispatch> >(obj));
    obj->AddRef();
    m_heldIDispatch.push_back(ref);
    return ref;
}

void FB::ActiveX::ActiveXBrowserHost::ReleaseAllHeldObjects()
{
    for (IDispatchRefList::iterator it(m_heldIDispatch.begin()); it != m_heldIDispatch.end(); ++it) {
        (*it)->getPtr()->Release();
    }
    m_heldIDispatch.clear();
}

void FB::ActiveX::ActiveXBrowserHost::Navigate( const std::string& url, const std::string& target )
{
    CComBSTR destURL(FB::utf8_to_wstring(url).c_str());
    CComVariant targetWin(FB::utf8_to_wstring(target).c_str());

    CComVariant vEmpty;

    HRESULT hr = m_webBrowser->Navigate(destURL, &vEmpty, &targetWin, &vEmpty, &vEmpty);
    assert(SUCCEEDED(hr));
}

