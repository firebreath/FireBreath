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
#include "BrowserObjectAPI.h"
#include "DOM/JSAPI_DOMWindow.h"
#include "variant_list.h"

#include "BrowserHostWrapper.h"

void FB::BrowserHostWrapper::htmlLog(const std::string& str)
{
    this->ScheduleAsyncCall(&FB::BrowserHostWrapper::AsyncHtmlLog,
            new FB::AsyncLogRequest(this, str));
}

void FB::BrowserHostWrapper::AsyncHtmlLog(void *logReq)
{
    FB::AsyncLogRequest *req = (FB::AsyncLogRequest*)logReq;
    try {
        FB::JSAPI_DOMWindow window = req->m_host->getDOMWindow();

        FB::JSObject obj = window.getProperty<FB::JSObject>("console");
        printf("Logging: %s\n", req->m_msg.c_str());
        obj->Invoke("log", FB::variant_list_of(req->m_msg));
    } catch (const std::exception &e) {
        printf("Exception: %s\n", e.what());
        // Fail silently; logging should not require success.
        return;
    }
    delete req;
}
