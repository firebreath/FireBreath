/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#include "IDispatchAPI.h"
#include "DOM/Window.h"
#include "DOM/Element.h"

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "Document.h"

using namespace FB::ActiveX::AXDOM;

Document::Document(const FB::JSObjectPtr& obj, IWebBrowser2 *web)
    : FB::ActiveX::AXDOM::Element(obj, web), FB::ActiveX::AXDOM::Node(obj, web), FB::DOM::Node(obj), FB::DOM::Element(obj),
      m_htmlDoc(FB::ptr_cast<IDispatchAPI>(obj)->getIDispatch()), m_webBrowser(web), FB::DOM::Document(obj)
{
}

Document::~Document()
{
}

FB::DOM::WindowPtr Document::getWindow() const
{
    CComQIPtr<IHTMLWindow2> htmlWin;
    m_htmlDoc->get_parentWindow(&htmlWin);
    CComQIPtr<IDispatch> windowDisp(htmlWin);
    FB::JSObjectPtr api(IDispatchAPI::create(htmlWin, FB::ptr_cast<ActiveXBrowserHost>(getJSObject()->getHost())));

    return FB::DOM::Window::create(api);
}

std::vector<FB::DOM::ElementPtr> Document::getElementsByTagName(const std::string& tagName) const
{
    CComQIPtr<IHTMLDocument3> doc(m_htmlDoc);
    CComPtr<IHTMLElementCollection> list;
    std::vector<FB::DOM::ElementPtr> tagList;
    CComBSTR tName(FB::utf8_to_wstring(tagName).c_str());
    if (doc) {
        doc->getElementsByTagName(tName, &list);
    } else {
        throw std::runtime_error("Could not get element by tag name");
    }
    long length(0);
    if (SUCCEEDED(list->get_length(&length))) {
        for (long i = 0; i < length; i++) {
            CComPtr<IDispatch> dispObj;
            CComVariant idx(i);
            list->item(idx, idx, &dispObj);
            FB::JSObjectPtr obj(IDispatchAPI::create(dispObj, FB::ptr_cast<ActiveXBrowserHost>(getJSObject()->getHost())));
            tagList.push_back(FB::DOM::Element::create(obj));
        }
    }
    return tagList;
}


FB::DOM::ElementPtr Document::getElementById(const std::string& elem_id) const
{
    CComQIPtr<IHTMLDocument3> doc3(m_htmlDoc);
    if (!doc3) {
        throw std::exception("Document does not support getElementById");
    }
    CComPtr<IHTMLElement> el(NULL);
    doc3->getElementById(CComBSTR(FB::utf8_to_wstring(elem_id).c_str()), &el);
    CComQIPtr<IDispatch> disp(el);
    FB::JSObjectPtr ptr(IDispatchAPI::create(disp, FB::ptr_cast<ActiveXBrowserHost>(getJSObject()->getHost())));
    return FB::DOM::Element::create(ptr);
}

FB::DOM::ElementPtr Document::createElement(const std::string &name) const
{
	CComPtr<IHTMLElement> el(NULL);
	HRESULT hr = m_htmlDoc->createElement(CComBSTR(FB::utf8_to_wstring(name).c_str()), &el);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create element!");
	}
	CComQIPtr<IDispatch> disp(el);
	FB::JSObjectPtr ptr(IDispatchAPI::create(disp, FB::ptr_cast<ActiveXBrowserHost>(getJSObject()->getHost())));
    return FB::DOM::Element::create(ptr);
}