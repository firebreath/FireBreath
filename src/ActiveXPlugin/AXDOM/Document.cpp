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

#include "Document.h"

using namespace AXDOM;

Document::Document(const FB::JSObjectPtr& obj, IWebBrowser2 *web)
    : m_htmlDoc(FB::ptr_cast<IDispatchAPI>(obj)->getIDispatch()), m_webBrowser(web), FB::DOM::Document(obj)
{
}

Document::~Document()
{
}

FB::DOM::WindowPtr Document::getWindow()
{
    CComQIPtr<IHTMLWindow2> htmlWin;
    m_htmlDoc->get_parentWindow(&htmlWin);
    CComQIPtr<IDispatch> windowDisp(htmlWin);
    FB::JSObjectPtr api(new IDispatchAPI(htmlWin.p, FB::ptr_cast<ActiveXBrowserHost>(m_element->host)));

    return FB::DOM::Window::create(api);
}

FB::DOM::ElementPtr Document::getElementById(const std::string& elem_id)
{
    CComQIPtr<IHTMLDocument3> doc3(m_htmlDoc);
    if (!doc3) {
        throw std::exception("Document does not support getElementById");
    }
    CComPtr<IHTMLElement> el(NULL);
    doc3->getElementById(CComBSTR(FB::utf8_to_wstring(elem_id).c_str()), &el);
    CComQIPtr<IDispatch> disp(el);
    FB::JSObjectPtr ptr(new IDispatchAPI(disp.p, FB::ptr_cast<ActiveXBrowserHost>(this->m_element->host)));
    return FB::DOM::Element::create(ptr);
}

std::vector<FB::DOM::ElementPtr> Document::getElementsByTagName(const std::string& tagName)
{
    CComQIPtr<IHTMLDocument3> doc(m_htmlDoc);
    CComPtr<IHTMLElementCollection> list;
    std::vector<FB::DOM::ElementPtr> tagList;
    doc->getElementsByTagName(CComBSTR(FB::utf8_to_wstring(tagName).c_str()), &list);
    long length(0);
    if (SUCCEEDED(list->get_length(&length))) {
        for (long i = 0; i < length; i++) {
            CComPtr<IDispatch> dispObj;
            CComVariant idx(i);
            list->item(idx, idx, &dispObj);
            FB::JSObjectPtr obj(new IDispatchAPI(dispObj.p, FB::ptr_cast<ActiveXBrowserHost>(this->m_element->host)));
            tagList.push_back(FB::DOM::Element::create(obj));
        }
    }
    return tagList;
}
