/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 14, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_SYNCBROWSERCALL
#define H_FB_SYNCBROWSERCALL

#include <vector>
#include <string>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include "APITypes.h"
#include "BrowserObjectAPI.h"

namespace FB {

    class SyncBrowserCall
    {
    public:
        static variant CallMethod(JSObjectPtr obj, const std::string& method,
            const std::vector<variant> &inParams);

    protected:
        SyncBrowserCall(JSObjectPtr obj, const std::string& method,
            const std::vector<variant> &inParams);

        static void asyncCallback(void *userData);

        JSObjectPtr m_obj;
        std::vector<variant> m_params;
        std::string m_methodName;
        variant m_result;
        bool m_returned;

        boost::condition_variable m_cond;
        boost::mutex m_mutex;
    };

};

#endif // H_FB_SYNCBROWSERCALL