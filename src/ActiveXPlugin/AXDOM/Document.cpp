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
#include "Document.h"
#include "Element.h"

using namespace AXDOM;

DocumentImpl::DocumentImpl(const FB::JSObject& obj, FB::DOM::Window &win, IHTMLDocument2 *doc)
    : m_htmlDoc(doc), m_win(win), FB::DOM::DocumentImpl(obj)
{
}

DocumentImpl::~DocumentImpl()
{
}

FB::DOM::Window DocumentImpl::getWindow()
{
    return m_win;
}

FB::DOM::Element DocumentImpl::getElementById(const std::string& elem_id)
{
    CComQIPtr<IHTMLDocument3> doc3(m_htmlDoc);
    if (!doc3) {
        throw std::exception("Document does not support getElementById");
    }
    CComPtr<IHTMLElement> el(NULL);
    doc3->getElementById(CComBSTR(FB::utf8_to_wstring(elem_id).c_str()), &el);
    CComQIPtr<IDispatch> disp(el);
    IDispatchAPIPtr ptr(new IDispatchAPI(disp.p, as_ActiveXBrowserHost(this->m_element->host)));
    FB::DOM::Element element(new AXDOM::ElementImpl(ptr, el));
    return element;
}

std::vector<FB::DOM::Element> DocumentImpl::getElementsByTagName(const std::string& tagName)
{
    CComQIPtr<IHTMLDocument3> doc(m_htmlDoc);
    CComPtr<IHTMLElementCollection> list;
    std::vector<FB::DOM::Element> tagList;
    doc->getElementsByTagName(CComBSTR(FB::utf8_to_wstring(tagName).c_str()), &list);
    long length(0);
    if (SUCCEEDED(list->get_length(&length))) {
        for (long i = 0; i < length; i++) {
            CComPtr<IDispatch> dispObj;
            CComVariant idx(i);
            list->item(idx, idx, &dispObj);
            FB::JSObject obj(new IDispatchAPI(dispObj.p, as_ActiveXBrowserHost(this->m_element->host)));
            CComQIPtr<IHTMLElement> el(dispObj);
            tagList.push_back(FB::DOM::Element(new AXDOM::ElementImpl(obj, el)));
        }
    }
    return tagList;
}