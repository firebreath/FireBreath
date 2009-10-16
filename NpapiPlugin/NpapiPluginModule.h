/**********************************************************\ 
Original Authors: Richard Bateman (taxilian)
                  Don Jordan (kc7zax)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPAPIPLUGINMODULE
#define H_NPAPIPLUGINMODULE

#include "APITypes.h"
#include "AutoPtr.h"
#include "NpapiTypes.h"

namespace FB { namespace Npapi {
    
    class NpapiPluginModule
    {
    public:
        NpapiPluginModule();
        virtual ~NpapiPluginModule(void);

        void setNetscapeFuncs(NPNetscapeFuncs *npnFuncs);

        NPNetscapeFuncs NPNFuncs;

    public:
        FB::variant getVariant(NPVariant *npVar);
        void getNPVariant(NPVariant *dst, FB::variant &var);

    public:
        // These are the NPN_ functions that don't require a NPP
        void Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor);
        void* MemAlloc(uint32 size);
        void MemFree(void* ptr);
        uint32 MemFlush(uint32 size);
        void ReloadPlugins(NPBool reloadPages);
        void ReleaseVariantValue(NPVariant *variant);
        NPIdentifier GetStringIdentifier(const NPUTF8 *name);
        void GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers);
        NPIdentifier GetIntIdentifier(int32_t intid);
        bool IdentifierIsString(NPIdentifier identifier);
        NPUTF8 *UTF8FromIdentifier(NPIdentifier identifier);
        int32_t IntFromIdentifier(NPIdentifier identifier);
        NPObject *RetainObject(NPObject *npobj);
        void ReleaseObject(NPObject *npobj);
    };

}; };

#endif