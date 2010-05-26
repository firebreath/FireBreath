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

#ifndef H_NPJAVASCRIPTOBJECT
#define H_NPJAVASCRIPTOBJECT

#include "APITypes.h"
#include "AutoPtr.h"
#include "JSAPI.h"
#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {
    class NPJavascriptObject :
        public NPObject
    {
    protected:
        static NPClass NPJavascriptObjectClass;
        FB::AutoPtr<FB::JSAPI> m_api;
        FB::AutoPtr<NpapiBrowserHost> m_browser;
        bool m_valid;

    public:
        static NPJavascriptObject *NewObject(NpapiBrowserHost *host, FB::JSAPI *api);
        void setAPI(FB::JSAPI *api, NpapiBrowserHost *host);
        NPJavascriptObject(NPP npp);
        virtual ~NPJavascriptObject(void);

    protected:
        virtual bool callSetEventListener(std::vector<variant> &args, bool add);

    protected:
        void Invalidate();
        bool HasMethod(NPIdentifier name);
        bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
        bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
        bool HasProperty(NPIdentifier name);
        bool GetProperty(NPIdentifier name, NPVariant *result);
        bool SetProperty(NPIdentifier name, const NPVariant *value);
        bool RemoveProperty(NPIdentifier name);
        bool Enumeration(NPIdentifier **value, uint32_t *count);
        bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);

    public:
        // Static methods referenced in the NPClass
        static NPObject *Allocate(NPP npp, NPClass *aClass);
        static void _Deallocate(NPObject *npobj);
        static void _Invalidate(NPObject *npobj);
        static bool _HasMethod(NPObject *npobj, NPIdentifier name);
        static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
        static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
        static bool _HasProperty(NPObject *npobj, NPIdentifier name);
        static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
        static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
        static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
        static bool _Enumeration(NPObject *npobj, NPIdentifier **value, uint32_t *count);
        static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    };

}; };

#endif
