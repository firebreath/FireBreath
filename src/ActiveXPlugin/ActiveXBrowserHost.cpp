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
        CComBSTR bstr;
        m_webBrowser->get_LocationURL(&bstr);
        m_location = FB::wstring_to_utf8(std::wstring(bstr.m_str));
    }
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
}

void ActiveXBrowserHost::ScheduleAsyncCall(void (*func)(void *), void *userData)
{
    if (m_hWnd != NULL)
        ::PostMessage(m_hWnd, WM_ASYNCTHREADINVOKE, NULL, 
            (LPARAM)new FB::AsyncFunctionCall(func, userData));
}

void *ActiveXBrowserHost::getContextID()
{
    return (void*)this;
}

void ActiveXBrowserHost::setWindow(HWND wnd)
{
    m_hWnd = wnd;
}

FB::DOM::Document ActiveXBrowserHost::getDOMDocument()
{
    FB::JSObject retObj(new IDispatchAPI(m_htmlDocDisp.p, as_ActiveXBrowserHost(shared_ptr())));
    return FB::DOM::Document(new FB::DOM::DocumentImpl(retObj));
}

FB::DOM::Window ActiveXBrowserHost::getDOMWindow()
{
    FB::JSObject retObj(new IDispatchAPI(m_htmlWin.p, as_ActiveXBrowserHost(shared_ptr())));
    return FB::DOM::Window(new FB::DOM::WindowImpl(retObj));
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

std::vector<FB::JSObject> ActiveXBrowserHost::getElementsByTagName(std::string tagName)
{
    CComQIPtr<IHTMLDocument3> doc(m_htmlDoc);
    CComPtr<IHTMLElementCollection> list;
    std::vector<FB::JSObject> tagList;
    doc->getElementsByTagName(CComBSTR(FB::utf8_to_wstring(tagName).c_str()), &list);
    long length(0);
    if (SUCCEEDED(list->get_length(&length))) {
        for (long i = 0; i < length; i++) {
            CComPtr<IDispatch> dispObj;
            CComVariant idx(i);
            list->item(idx, idx, &dispObj);
            tagList.push_back(FB::JSObject(new IDispatchAPI(dispObj.p, as_ActiveXBrowserHost(shared_ptr()))));
        }
    }
    return tagList;
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
        retVal = FB::JSObject(new IDispatchAPI(cVar->pdispVal, as_ActiveXBrowserHost(shared_ptr()))); 
        break;

    case VT_ERROR:
    case VT_BOOL:
        converted.ChangeType(VT_BOOL, cVar);
        retVal = (converted.boolVal == VARIANT_TRUE) ? true : false;
        break;

    case VT_EMPTY:
    case VT_NULL:
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
        || var.get_type() == typeid(long)
        || var.get_type() == typeid(short)
        || var.get_type() == typeid(char)
        || var.get_type() == typeid(unsigned int)
        || var.get_type() == typeid(unsigned short)
        || var.get_type() == typeid(unsigned char)) {
        // Integer type
        outVar = var.convert_cast<long>();

    } else if (var.get_type() == typeid(double)
        || var.get_type() == typeid(float)) {
        outVar = var.convert_cast<double>();

    } else if (var.get_type() == typeid(bool)) {
        outVar = var.convert_cast<bool>();

    } else if (var.get_type() == typeid(std::string)
            || var.get_type() == typeid(std::wstring)) {
        std::wstring wstr = var.convert_cast<std::wstring>();
        CComBSTR bStr(wstr.c_str());
        outVar = bStr;

    } else if (var.get_type() == typeid(FB::VariantList)) {
        DOM::Node outArr = this->getDOMWindow()->createArray();
        FB::VariantList inArr = var.cast<FB::VariantList>();
        for (FB::VariantList::iterator it = inArr.begin(); it != inArr.end(); it++) {
            FB::VariantList vl = list_of(*it);
            outArr->callMethod<void>("push", vl);
        }
        IDispatchAPIPtr api = as_IDispatchAPI(outArr->getJSObject());
        if (api) {
            outVar = api->getIDispatch();
        }

    } else if (var.get_type() == typeid(FB::VariantMap)) {
        DOM::Node out = this->getDOMWindow()->createMap();
        FB::VariantMap inMap = var.cast<FB::VariantMap>();
        for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); it++) {
            out->setProperty(it->first, it->second);
        }
        IDispatchAPIPtr api = as_IDispatchAPI(out->getJSObject());
        if (api) {
            outVar = api->getIDispatch();
        }

    } else if (var.get_type() == typeid(FB::JSObject)) {
        IDispatchAPIPtr api = as_IDispatchAPI(var.cast<JSOutObject>());
        if (api) {
            outVar = api->getIDispatch();
        } else {
            outVar = COMJavascriptObject::NewObject(as_ActiveXBrowserHost(shared_ptr()), var.cast<FB::JSObject>());
        }

    } else if (var.get_type() == typeid(JSOutObject)) {
        IDispatchAPIPtr api = as_IDispatchAPI(var.cast<JSOutObject>());
        if (api) {
            outVar = api->getIDispatch();
        } else {
            outVar = COMJavascriptObject::NewObject(as_ActiveXBrowserHost(shared_ptr()), var.cast<JSOutObject>());
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
