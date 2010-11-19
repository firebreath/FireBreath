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

#include "ActiveXBrowserHost.h"
#include "axstream.h"
#include "COMJavascriptObject.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "AsyncFunctionCall.h"
#include <boost/assign.hpp>
#include "AXDOM/Window.h"
#include "AXDOM/Document.h"
#include "AXDOM/Element.h"
#include "AXDOM/Node.h"

#include <boost/config.hpp>

using boost::assign::list_of;

#include "Win/PluginWindowWin.h"

using namespace FB;

ActiveXBrowserHost::ActiveXBrowserHost(IWebBrowser2 *doc)
    : m_webBrowser(doc)
{
    if (m_webBrowser.p) {
        m_webBrowser->get_Document(&m_htmlDocDisp);
        m_htmlDoc = m_htmlDocDisp;
        m_htmlDoc->get_parentWindow(&m_htmlWin);
        m_htmlWinDisp = m_htmlWin;
    }
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
}

void ActiveXBrowserHost::ScheduleAsyncCall(void (*func)(void *), void *userData)
{
    if (m_hWnd != NULL) {
        FBLOG_TRACE("ActiveXHost", "Scheduling async call for main thread");
        ::PostMessage(m_hWnd, WM_ASYNCTHREADINVOKE, NULL, 
            (LPARAM)new FB::AsyncFunctionCall(func, userData));
    }
}

void *ActiveXBrowserHost::getContextID()
{
    return (void*)this;
}

void ActiveXBrowserHost::setWindow(HWND wnd)
{
    m_hWnd = wnd;
}

FB::DOM::WindowPtr ActiveXBrowserHost::_createWindow(const FB::JSObjectPtr& obj)
{
    return FB::DOM::WindowPtr(new AXDOM::Window(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::DocumentPtr ActiveXBrowserHost::_createDocument(const FB::JSObjectPtr& obj)
{
    return FB::DOM::DocumentPtr(new AXDOM::Document(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::ElementPtr ActiveXBrowserHost::_createElement(const FB::JSObjectPtr& obj)
{
    return FB::DOM::ElementPtr(new AXDOM::Element(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

FB::DOM::NodePtr ActiveXBrowserHost::_createNode(const FB::JSObjectPtr& obj)
{
    return FB::DOM::NodePtr(new AXDOM::Node(ptr_cast<IDispatchAPI>(obj), m_webBrowser));
}

void ActiveXBrowserHost::initDOMObjects()
{
    if (!m_window) {
        m_window = DOM::Window::create(FB::JSObjectPtr(new IDispatchAPI(m_htmlWin.p, ptr_cast<ActiveXBrowserHost>(shared_ptr()))));
        m_document = DOM::Document::create(FB::JSObjectPtr(new IDispatchAPI(m_htmlDocDisp.p, ptr_cast<ActiveXBrowserHost>(shared_ptr()))));
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

void ActiveXBrowserHost::evaluateJavaScript(const std::string &script)
{
    VARIANT res;

    if(m_htmlDoc.p == NULL) {
        throw FB::script_error("Can't execute JavaScript: Window is NULL");
    }

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
    case VT_UI4:
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
        converted.ChangeType(VT_I4, cVar);
        retVal = (long)converted.lVal;
        break;

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
        retVal = FB::JSObjectPtr(new IDispatchAPI(cVar->pdispVal, ptr_cast<ActiveXBrowserHost>(shared_ptr()))); 
        break;

    case VT_ERROR:
    case VT_BOOL:
        converted.ChangeType(VT_BOOL, cVar);
        retVal = (converted.boolVal == VARIANT_TRUE) ? true : false;
        break;

    case VT_NULL:
        retVal = FB::variant_detail::null();
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
    VariantInit(dest);
    if (var.empty()) {
        // Already empty
        return;

    } else if (var.get_type() == typeid(int)
        || var.get_type() == typeid(short)
        || var.get_type() == typeid(char)
        
        || var.get_type() == typeid(unsigned short)
        || var.get_type() == typeid(unsigned char)) {
        // Integer type
        outVar = var.convert_cast<int32_t>();

    } else if (var.get_type() == typeid(double)
        || var.get_type() == typeid(float)
        || var.get_type() == typeid(unsigned int)
        || var.get_type() == typeid(long)
        || var.get_type() == typeid(unsigned long)
#if !defined(BOOST_NO_LONG_LONG)
        || var.get_type() == typeid(long long)
        || var.get_type() == typeid(unsigned long long)
#endif
        ) {
        // Arithmetic types whose range doesn't fit into int32 
        // (ignoring LLP64 vs. other data models, refactoring will be in 1.4)
        outVar = var.convert_cast<double>();

    } else if (var.get_type() == typeid(bool)) {
        outVar = var.convert_cast<bool>();

    } else if (var.get_type() == typeid(std::string)
            || var.get_type() == typeid(std::wstring)) {
        std::wstring wstr = var.convert_cast<std::wstring>();
        CComBSTR bStr(wstr.c_str());
        outVar = bStr;

    } else if (var.get_type() == typeid(FB::VariantList)) {
        FB::JSObjectPtr outArr = this->getDOMWindow()->createArray();
        FB::VariantList inArr = var.cast<FB::VariantList>();
        for (FB::VariantList::iterator it = inArr.begin(); it != inArr.end(); it++) {
            FB::VariantList vl = list_of(*it);
            outArr->Invoke("push", vl);
        }
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(outArr);
        if (api) {
            outVar = api->getIDispatch();
        }

    } else if (var.get_type() == typeid(FB::VariantMap)) {
        FB::JSObjectPtr out = this->getDOMWindow()->createMap();
        FB::VariantMap inMap = var.cast<FB::VariantMap>();
        for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); it++) {
            out->SetProperty(it->first, it->second);
        }
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(out);
        if (api) {
            outVar = api->getIDispatch();
        }

    } else if (var.get_type() == typeid(FB::JSObject)) {
        FB::JSObjectPtr obj(var.cast<FB::JSObjectPtr>());
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                outVar = COMJavascriptObject::NewObject(ptr_cast<ActiveXBrowserHost>(shared_ptr()), var.cast<FB::JSObjectPtr>());
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }

    } else if (var.get_type() == typeid(JSAPIPtr)) {
        FB::JSAPIPtr obj(var.cast<FB::JSAPIPtr>());
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                outVar = COMJavascriptObject::NewObject(ptr_cast<ActiveXBrowserHost>(shared_ptr()), var.cast<JSAPIPtr>());
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }
    }

    outVar.Detach(dest);
}

FB::BrowserStream* ActiveXBrowserHost::createStream(const std::string& url, FB::PluginEventSink* callback, 
                                    bool cache, bool seekable, size_t internalBufferSize )
{
    ActiveXStream* stream = new ActiveXStream( url, cache, seekable, internalBufferSize );
    stream->AttachObserver( callback );

    if ( stream->init() )
    {
        StreamCreatedEvent ev(stream);
        stream->SendEvent( &ev );
        if ( seekable ) stream->signalOpened();
        return stream;
    }
    else
    {
        delete stream;
        return 0;
    }
}
