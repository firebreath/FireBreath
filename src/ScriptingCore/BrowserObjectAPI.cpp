/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "AsyncBrowserCall.h"
#include "SyncBrowserCall.h"

using namespace FB;

BrowserObjectAPI::BrowserObjectAPI(BrowserHost h) :
    host(h)
{
}

BrowserObjectAPI::~BrowserObjectAPI(void)
{
}

variant BrowserObjectAPI::InvokeMainThread(const std::string& methodName, const std::vector<variant>& args)
{
    assert(!host->isMainThread());
    return FB::SyncBrowserCall::CallMethod(ptr_cast<BrowserObjectAPI>(shared_ptr()), methodName, args);
}

void BrowserObjectAPI::InvokeAsync(const std::string& methodName, const std::vector<variant>& args)
{
    FB::AsyncBrowserCall::CallMethod(ptr_cast<BrowserObjectAPI>(shared_ptr()), methodName, args);
}
