/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 14, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "SyncBrowserCall.h"

using namespace FB;

SyncBrowserCall::SyncBrowserCall(JSObject obj, const std::string& method,
                                   const std::vector<variant> &inParams) :
    m_obj(obj), m_params(inParams), m_methodName(method), m_returned(false)
{
}

variant SyncBrowserCall::CallMethod(JSObject obj, const std::string& method,
            const std::vector<variant> &inParams)
{
    variant result;
    SyncBrowserCall *call = new SyncBrowserCall(obj, method, inParams);
    {
        boost::unique_lock<boost::mutex> lock(call->m_mutex);
        obj->host->ScheduleAsyncCall(&SyncBrowserCall::asyncCallback, call);

        while (!call->m_returned) {
            call->m_cond.wait(lock);
        }
        result = call->m_result;
    }
    delete call;
    if (result.get_type() == typeid(FB::script_error)) {
        throw FB::script_error(result.cast<const FB::script_error>().what());
    }
    return result;
}

void SyncBrowserCall::asyncCallback(void *userData)
{
    SyncBrowserCall *call = static_cast<SyncBrowserCall *>(userData);
    {
        // Make sure the lock goes out of scope before we finish
        boost::lock_guard<boost::mutex> lock(call->m_mutex);
        try 
        {
            call->m_result = call->m_obj->Invoke(call->m_methodName, call->m_params);
        }
        catch(const FB::script_error& e)
        {
            call->m_result = new FB::script_error(e.what());
        }
        call->m_cond.notify_one();
    }
}
