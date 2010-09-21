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

#include "Window.h"

using namespace AXDOM;

WindowImpl::WindowImpl(FB::JSObject obj, IWebBrowser2 *web)
    : m_webBrowser(web), FB::DOM::WindowImpl(obj)
{
    m_webBrowser->get_Document(&m_htmlDocDisp);
    m_htmlDoc = m_htmlDocDisp;
    m_htmlDoc->get_parentWindow(&m_htmlWin);
}

WindowImpl::~WindowImpl()
{
}

FB::DOM::Document WindowImpl::getDocument()
{
    IDispatchAPIPtr docAPI(new IDispatchAPI(m_htmlDocDisp, as_ActiveXBrowserHost(this->m_element->host)));
    return FB::DOM::Document(new DocumentImpl(docAPI, window(), m_htmlDoc));
}

void WindowImpl::alert(const std::string& str)
{
    m_htmlWin->alert(CComBSTR(FB::utf8_to_wstring(str).c_str()));
}