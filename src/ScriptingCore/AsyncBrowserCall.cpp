/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 22, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "AsyncBrowserCall.h"

using namespace FB;

void AsyncBrowserCall::asyncCallback(void *userData)
{
    AsyncBrowserCall *call = static_cast<AsyncBrowserCall *>(userData);
    
    try 
    {
        call->m_obj->Invoke(call->m_methodName, call->m_params);
    }
    catch(const FB::script_error&)
    {
        // can't really do anything useful here
    }
    delete call;
}

void AsyncBrowserCall::CallMethod(BrowserObjectAPI *obj, std::string method,
            std::vector<variant> &inParams)
{
    AsyncBrowserCall *call = new AsyncBrowserCall(obj, method, inParams);
    obj->host->ScheduleAsyncCall(&AsyncBrowserCall::asyncCallback, call);
}

AsyncBrowserCall::AsyncBrowserCall(BrowserObjectAPI *obj, std::string method,
                                   std::vector<variant> &inParams) :
    m_obj(obj), m_params(inParams), m_methodName(method)
{
}

AsyncBrowserCall::~AsyncBrowserCall(void)
{
}
