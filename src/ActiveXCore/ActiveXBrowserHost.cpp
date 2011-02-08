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
#include "ActiveXBrowserHost.h"
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

using boost::assign::list_of;
using namespace FB;
using namespace FB::ActiveX;

namespace
{   
    template<class T>
    ComVariantBuilderMap::value_type makeBuilderEntry()
    {
        return ComVariantBuilderMap::value_type(&typeid(T), select_ccomvariant_builder::select<T>());
    }
    
    ComVariantBuilderMap makeComVariantBuilderMap()
    {
        ComVariantBuilderMap tdm;
        tdm.insert(makeBuilderEntry<bool>());
        tdm.insert(makeBuilderEntry<char>());
        tdm.insert(makeBuilderEntry<unsigned char>());
        tdm.insert(makeBuilderEntry<short>());
        tdm.insert(makeBuilderEntry<unsigned short>());
        tdm.insert(makeBuilderEntry<int>());
        tdm.insert(makeBuilderEntry<unsigned int>());
        tdm.insert(makeBuilderEntry<long>());
        tdm.insert(makeBuilderEntry<unsigned long>());
        
#ifndef BOOST_NO_LONG_LONG
        tdm.insert(makeBuilderEntry<long long>());
        tdm.insert(makeBuilderEntry<unsigned long long>());
#endif
        
        tdm.insert(makeBuilderEntry<float>());
        tdm.insert(makeBuilderEntry<double>());
        
        tdm.insert(makeBuilderEntry<std::string>());
        tdm.insert(makeBuilderEntry<std::wstring>());
        
        tdm.insert(makeBuilderEntry<FB::VariantList>());
        tdm.insert(makeBuilderEntry<FB::VariantMap>());
        tdm.insert(makeBuilderEntry<FB::JSAPIPtr>());
        tdm.insert(makeBuilderEntry<FB::JSAPIWeakPtr>());
        tdm.insert(makeBuilderEntry<FB::JSObjectPtr>());

        tdm.insert(makeBuilderEntry<FB::FBVoid>());
        tdm.insert(makeBuilderEntry<FB::FBNull>());
        
        return tdm;
    }
    
    const ComVariantBuilderMap& getComVariantBuilderMap()
    {
        static const ComVariantBuilderMap tdm = makeComVariantBuilderMap();
        return tdm;
    }
}

ActiveXBrowserHost::ActiveXBrowserHost(IWebBrowser2 *doc, IOleClientSite* site)
    : m_spClientSite(site), m_webBrowser(doc), m_messageWin(new FB::WinMessageWindow())
{
    if (m_webBrowser) {
        m_webBrowser->get_Document(&m_htmlDocDisp);
        m_htmlDoc = m_htmlDocDisp;
        m_htmlDoc->get_parentWindow(&m_htmlWin);
        m_htmlWinDisp = m_htmlWin;
    }
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
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
    if (!m_window) {
		m_window = DOM::Window::create(IDispatchAPI::create(m_htmlWin, ptr_cast<ActiveXBrowserHost>(shared_ptr())));
        m_document = DOM::Document::create(IDispatchAPI::create(m_htmlDocDisp, ptr_cast<ActiveXBrowserHost>(shared_ptr())));
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
    return DOM::Document::create(IDispatchAPI::create(htmlElement, ptr_cast<ActiveXBrowserHost>(shared_ptr())));
}

void ActiveXBrowserHost::evaluateJavaScript(const std::string &script)
{
    if(!m_htmlDoc) {
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
    // First, make sure that no async calls are made while we're shutting down
    boost::upgrade_lock<boost::shared_mutex> _l(m_xtmutex);
    // Next, kill the message window so that none that have been made go through
    m_messageWin.reset();

    // Finally, run the main browser shutdown, which will fire off any cross-thread
    // calls that somehow haven't made it through yet
	BrowserHost::shutdown();

    // Once that's done let's release any ActiveX resources that the browserhost
    // is holding
	m_spClientSite.Release();
	m_htmlDoc.Release();
	m_htmlDocDisp.Release();
	m_htmlWin.Release();
	m_webBrowser.Release();
	m_htmlWinDisp.Release();
	m_window.reset();
	m_document.reset();
    DoDeferredRelease();
    assert(m_deferredObjects.empty());
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
		retVal = FB::JSObjectPtr(IDispatchAPI::create(cVar->pdispVal, ptr_cast<ActiveXBrowserHost>(shared_ptr()))); 
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
    
    outVar = (it->second)(FB::ptr_cast<ActiveXBrowserHost>(shared_ptr()), var);

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

void ActiveXBrowserHost::DoDeferredRelease() const
{
    assertMainThread();
    IDispatch* deferred;
    while (m_deferredObjects.try_pop(deferred)) {
        deferred->Release();
    }
}


void FB::ActiveX::ActiveXBrowserHost::deferred_release( IDispatch* m_obj ) const
{
    m_deferredObjects.push(m_obj);
}

