/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "Document.h"
#include "IDispatchAPI.h"

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "Window.h"

using namespace FB::ActiveX::AXDOM;

Window::Window(FB::JSObjectPtr obj, IWebBrowser2 *web)
    : FB::ActiveX::AXDOM::Node(obj, web), FB::DOM::Window(obj), FB::DOM::Node(obj),
    m_webBrowser(web), m_htmlWin(FB::ptr_cast<IDispatchAPI>(obj)->getIDispatch())
{
    if (!m_htmlWin) {
        throw new std::bad_cast("This is not a valid Window object");
    }
}

Window::~Window()
{
}

FB::DOM::DocumentPtr Window::getDocument() const
{
    CComPtr<IHTMLDocument2> htmlDoc;
    m_htmlWin->get_document(&htmlDoc);
    CComQIPtr<IDispatch> htmlDocDisp(htmlDoc);
    FB::JSObjectPtr docAPI(IDispatchAPI::create(htmlDocDisp, FB::ptr_cast<ActiveXBrowserHost>(this->m_element->getHost())));
    return FB::DOM::Document::create(docAPI);
}

void Window::alert(const std::string& str) const
{
    m_htmlWin->alert(CComBSTR(FB::utf8_to_wstring(str).c_str()));
}

std::string Window::getLocation() const
{
    CComBSTR bstr;
    m_webBrowser->get_LocationURL(&bstr);
    return FB::wstring_to_utf8(std::wstring(bstr.m_str));
}

