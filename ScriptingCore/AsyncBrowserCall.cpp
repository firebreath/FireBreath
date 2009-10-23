/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 22, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "AsyncBrowserCall.h"

using namespace FB;

void AsyncBrowserCall::asyncCallback(void *userData)
{
    AsyncBrowserCall *call = static_cast<AsyncBrowserCall *>(userData);
    call->m_obj->Invoke(call->m_methodName, call->m_params);
}

void AsyncBrowserCall::CallMethod(EventHandlerObject *obj, std::string method,
            std::vector<variant> &inParams)
{
    AsyncBrowserCall *call = new AsyncBrowserCall(obj, method, inParams);
    obj->host->ScheduleAsyncCall(&AsyncBrowserCall::asyncCallback, call);
}

AsyncBrowserCall::AsyncBrowserCall(EventHandlerObject *obj, std::string method,
                                   std::vector<variant> &inParams) :
    m_obj(obj), m_methodName(method), m_params(inParams)
{
}

AsyncBrowserCall::~AsyncBrowserCall(void)
{
}
