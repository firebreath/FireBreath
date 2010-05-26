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

using namespace FB;

BrowserObjectAPI::BrowserObjectAPI(BrowserHostWrapper *h) :
    host(h)
{
}

BrowserObjectAPI::~BrowserObjectAPI(void)
{
}

void BrowserObjectAPI::InvokeAsync(std::string methodName, std::vector<variant>& args)
{
    FB::AsyncBrowserCall::CallMethod(this, methodName, args);
}