/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "Element.h"

using namespace FB::ActiveX::AXDOM;

Element::Element(const FB::JSObjectPtr& element, IWebBrowser *web)
    : FB::ActiveX::AXDOM::Node(element, web), FB::DOM::Node(element), FB::DOM::Element(element),
      m_axDisp(FB::ptr_cast<IDispatchAPI>(element)->getIDispatch()), m_webBrowser(web)
{
    if (!m_axDisp)
        throw std::bad_cast("This is not a valid object");
}

Element::~Element()
{
}

std::vector<FB::DOM::ElementPtr> Element::getElementsByTagName(const std::string& tagName) const
{
    CComQIPtr<IHTMLElement2> elem(m_axDisp);
    CComQIPtr<IHTMLDocument3> doc(m_axDisp);
    CComPtr<IHTMLElementCollection> list;
    std::vector<FB::DOM::ElementPtr> tagList;
    CComBSTR tName(FB::utf8_to_wstring(tagName).c_str());
    if (elem) {
        elem->getElementsByTagName(tName, &list);
    } else if (doc) {
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

std::string FB::ActiveX::AXDOM::Element::getStringAttribute( const std::string& attr ) const
{
    CComQIPtr<IHTMLElement> elem(m_axDisp);
    CComQIPtr<IHTMLDocument5> doc(m_axDisp);
    CComVariant var;
    HRESULT hr = S_OK;
    if (elem) {
        hr = elem->getAttribute(CComBSTR(FB::utf8_to_wstring(attr).c_str()), 0, &var);
        return FB::ptr_cast<ActiveXBrowserHost>(getJSObject()->getHost())->getVariant(&var).convert_cast<std::string>();
    } else {
        return getProperty<std::string>(attr);
    }
}

