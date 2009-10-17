/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPOBJECTAPI
#define H_NPOBJECTAPI

#include "npapi.h"
#include "npruntime.h"
#include "JSAPI.h"
#include "AutoPtr.h"
#include "EventHandlerObject.h"
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {

    class NPObjectAPI :
        public FB::EventHandlerObject
    {
    public:
        NPObjectAPI(NPObject *, NpapiBrowserHost *);
        virtual ~NPObjectAPI(void);

        void *getEventId() { return (void*)obj; }
        void *getEventContext() { return browser->getContextID(); };
    protected:
        FB::AutoPtr<NpapiBrowserHost> browser;
        NPObject *obj;

    public:
        // Methods to query existance of members on the API
        bool HasMethod(std::string methodName);
        bool HasProperty(std::string propertyName);
        bool HasProperty(int idx);
        bool HasEvent(std::string eventName);

        // Methods to manage properties on the API
        variant GetProperty(std::string propertyName);
        void SetProperty(std::string propertyName, const variant value);
        variant GetProperty(int idx);
        void SetProperty(int idx, const variant value);

        // Methods to manage methods on the API
        variant Invoke(std::string methodName, std::vector<variant>& args);
    };

}; };
#endif