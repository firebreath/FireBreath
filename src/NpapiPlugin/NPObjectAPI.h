/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPOBJECTAPI
#define H_NPOBJECTAPI

#include "NpapiTypes.h"
#include "npruntime.h"
#include "JSAPI.h"
#include "AutoPtr.h"
#include "BrowserObjectAPI.h"
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {

    class NPObjectAPI :
        public FB::BrowserObjectAPI
    {
    public:
        NPObjectAPI(NPObject *, NpapiBrowserHost *);
        virtual ~NPObjectAPI(void);

        void *getEventId() { return (void*)obj; }
        void *getEventContext() { return browser->getContextID(); };
        NPObject *getNPObject() { return obj; }

        // Enumerate members
        void getMemberNames(std::vector<std::string> &nameVector);
        size_t getMemberCount();
    protected:
        FB::AutoPtr<NpapiBrowserHost> browser;
        NPObject *obj;

    public:
        // Methods to query existance of members on the API
        bool HasMethod(const std::string& methodName);
        bool HasProperty(const std::string& propertyName);
        bool HasProperty(int idx);
        bool HasEvent(const std::string& eventName);

        // Methods to manage properties on the API
        variant GetProperty(const std::string& propertyName);
        void SetProperty(const std::string& propertyName, const variant& value);
        variant GetProperty(int idx);
        void SetProperty(int idx, const variant& value);

        // Methods to manage methods on the API
        variant Invoke(const std::string& methodName, const std::vector<variant>& args);
    };

}; };
#endif