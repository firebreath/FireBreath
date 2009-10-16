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
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {

    class NPObjectAPI :
        public FB::JSAPI
    {
    public:
        NPObjectAPI(NPObject *, NpapiBrowserHost *);
        virtual ~NPObjectAPI(void);

    protected:
        FB::AutoPtr<NpapiBrowserHost> browser;
        NPObject *obj;

    public:
        // Methods to query existance of members on the API
        virtual bool HasMethod(std::string methodName);
        virtual bool HasProperty(std::string propertyName);
        virtual bool HasProperty(int idx);
        virtual bool HasEvent(std::string eventName);

        // Methods to manage properties on the API
        virtual variant GetProperty(std::string propertyName);
        virtual void SetProperty(std::string propertyName, const variant value);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant value);

        // Methods to manage methods on the API
        virtual variant Invoke(std::string methodName, std::vector<variant>& args);
    };

}; };
#endif