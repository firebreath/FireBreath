/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BrowserObjectAPI
#define H_FB_BrowserObjectAPI

#include "JSAPI.h"
#include "BrowserHostWrapper.h"
#include "AutoPtr.h"

namespace FB
{
    class BrowserObjectAPI : public FB::JSAPI
    {
    public:
        BrowserObjectAPI(BrowserHostWrapper *h);
        virtual ~BrowserObjectAPI();

        virtual void *getEventId() { return NULL; }
        virtual void *getEventContext() { return NULL; }

        virtual void InvokeAsync(std::string methodName, std::vector<variant>& args);

    public:
        AutoPtr<BrowserHostWrapper> host;
    };
};
#endif