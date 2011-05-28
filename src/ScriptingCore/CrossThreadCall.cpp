/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 2, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "CrossThreadCall.h"

using namespace FB;

void CrossThreadCall::syncCallbackFunctor(void *userData)
{
    std::auto_ptr<CrossThreadCallWeakPtr> callWeak(static_cast<CrossThreadCallWeakPtr*>(userData));
    if (CrossThreadCallPtr call = callWeak->lock()) {
        try 
        {
            call->funct->call();
        }
        catch(const FB::script_error& e)
        {
            call->m_result = variant(boost::make_shared<FB::script_error>(e.what()), true);
        }
        // Make sure the lock goes out of scope before we finish
        boost::lock_guard<boost::mutex> lock(call->m_mutex);
        call->m_returned = true;
        call->m_cond.notify_one();
    }
}

void CrossThreadCall::asyncCallbackFunctor(void *userData)
{
    std::auto_ptr<CrossThreadCall> call(static_cast<CrossThreadCall *>(userData));

    try {        
        call->funct->call();
    } catch (const FB::script_error& ex) {
        // we can't sensibly handle it here
        FB_UNUSED_VARIABLE(ex);
    } catch (const FB::bad_variant_cast& ex) {
        // we can't sensibly handle it here
        FB_UNUSED_VARIABLE(ex);
    }
}

