/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Jan 23, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <cstdio>
#include <cassert>
#include "JSObject.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "logging.h"

#include "BrowserHost.h"

void FB::BrowserHost::htmlLog(const std::string& str)
{
    FBLOG_INFO("BrowserHost", "Logging to HTML: " << str);
    this->ScheduleAsyncCall(&FB::BrowserHost::AsyncHtmlLog,
            new FB::AsyncLogRequest(shared_ptr(), str));
}

void FB::BrowserHost::AsyncHtmlLog(void *logReq)
{
    FB::AsyncLogRequest *req = (FB::AsyncLogRequest*)logReq;
    try {
        FB::DOM::WindowPtr window = req->m_host->getDOMWindow();

        if (window->getJSObject()->HasProperty("console")) {
            FB::JSObjectPtr obj = window->getProperty<FB::JSObjectPtr>("console");
            printf("Logging: %s\n", req->m_msg.c_str());
            obj->Invoke("log", FB::variant_list_of(req->m_msg));
        }
    } catch (const std::exception &) {
        // printf("Exception: %s\n", e.what());
        // Fail silently; logging should not require success.
        FBLOG_TRACE("BrowserHost", "Logging to browser console failed");
        return;
    }
    delete req;
}

void FB::BrowserHost::evaluateJavaScript(const std::wstring &script)
{
    evaluateJavaScript(FB::wstring_to_utf8(script));
}

FB::DOM::WindowPtr FB::BrowserHost::_createWindow(const FB::JSObjectPtr& obj)
{
    return FB::DOM::WindowPtr(new FB::DOM::Window(obj));
}

FB::DOM::DocumentPtr FB::BrowserHost::_createDocument(const FB::JSObjectPtr& obj)
{
    return FB::DOM::DocumentPtr(new FB::DOM::Document(obj));
}

FB::DOM::ElementPtr FB::BrowserHost::_createElement(const FB::JSObjectPtr& obj)
{
    return FB::DOM::ElementPtr(new FB::DOM::Element(obj));
}

FB::DOM::NodePtr FB::BrowserHost::_createNode(const FB::JSObjectPtr& obj)
{
    return FB::DOM::NodePtr(new FB::DOM::Node(obj));
}


void FB::BrowserHost::assertMainThread()
{
#ifdef _DEBUG
    if (!isMainThread()) {
        FBLOG_FATAL("BrowserHost", "Trying to call something from the wrong thread!");
    }
    assert(isMainThread());
#endif
}

bool FB::BrowserHost::isMainThread()
{
    return m_threadId == boost::this_thread::get_id();
}