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
#include "BrowserObjectAPI.h"
#include "DOM/Window.h"
#include "variant_list.h"

#include "BrowserHostWrapper.h"

void FB::BrowserHostWrapper::htmlLog(const std::string& str)
{
    this->ScheduleAsyncCall(&FB::BrowserHostWrapper::AsyncHtmlLog,
            new FB::AsyncLogRequest(shared_ptr(), str));
}

void FB::BrowserHostWrapper::AsyncHtmlLog(void *logReq)
{
    FB::AsyncLogRequest *req = (FB::AsyncLogRequest*)logReq;
    try {
        FB::DOM::Window window = req->m_host->getDOMWindow();

        FB::JSObject obj = window->getProperty<FB::JSObject>("console");
        printf("Logging: %s\n", req->m_msg.c_str());
        obj->Invoke("log", FB::variant_list_of(req->m_msg));
    } catch (const std::exception &e) {
        printf("Exception: %s\n", e.what());
        // Fail silently; logging should not require success.
        return;
    }
    delete req;
}

void FB::BrowserHostWrapper::evaluateJavaScript(const std::wstring &script)
{
    evaluateJavaScript(FB::wstring_to_utf8(script));
}

std::vector<FB::JSObject> FB::BrowserHostWrapper::getElementsByTagName(std::wstring tagName)
{
    return getElementsByTagName(FB::wstring_to_utf8(tagName));
}


void FB::BrowserHostWrapper::assertMainThread()
{
#ifdef _DEBUG
    assert(isMainThread());
#endif
}

bool FB::BrowserHostWrapper::isMainThread()
{
    return m_threadId == boost::this_thread::get_id();
}