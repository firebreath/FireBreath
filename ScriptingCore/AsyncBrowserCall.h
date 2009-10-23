/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 22, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_NPAPI_ASYNCBROWSERCALL
#define H_FB_NPAPI_ASYNCBROWSERCALL

#include <vector>
#include <string>
#include "APITypes.h"
#include "AutoPtr.h"
#include "EventHandlerObject.h"

namespace FB {

    class AsyncBrowserCall
    {
    public:
        virtual ~AsyncBrowserCall(void);
        static void CallMethod(EventHandlerObject *obj, std::string method,
            std::vector<variant> &inParams);

    protected:
        AsyncBrowserCall(EventHandlerObject *obj, std::string method,
            std::vector<variant> &inParams);

        static void AsyncBrowserCall::asyncCallback(void *userData);

        AutoPtr<EventHandlerObject> m_obj;
        std::vector<variant> m_params;
        std::string m_methodName;
    };

};

#endif