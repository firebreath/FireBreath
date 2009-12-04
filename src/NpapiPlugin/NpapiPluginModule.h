/**********************************************************\ 
Original Authors: Richard Bateman (taxilian)
Don Jordan (kc7zax)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

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
        static NpapiPluginModule *Default;
        NpapiPluginModule();
        virtual ~NpapiPluginModule(void);

        void setNetscapeFuncs(NPNetscapeFuncs *npnFuncs);
        void getPluginFuncs(NPPluginFuncs* pFuncs);

        NPNetscapeFuncs NPNFuncs;

    public:
        FB::variant getVariant(NPVariant *npVar);
        void getNPVariant(NPVariant *dst, FB::variant &var);

    public:
        // These are the NPN_ functions that don't require a NPP
        void Version(int* plugin_major, int* plugin_minor, int* netscape_major,
            int* netscape_minor);
        void* MemAlloc(uint32 size);
        void MemFree(void* ptr);
        uint32 MemFlush(uint32 size);
        void ReloadPlugins(NPBool reloadPages);
        void ReleaseVariantValue(NPVariant *variant);
        NPIdentifier GetStringIdentifier(const NPUTF8 *name);
        void GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount,
            NPIdentifier *identifiers);
        NPIdentifier GetIntIdentifier(int32_t intid);
        bool IdentifierIsString(NPIdentifier identifier);
        NPUTF8 *UTF8FromIdentifier(NPIdentifier identifier);
        std::string StringFromIdentifier(NPIdentifier identifier);
        int32_t IntFromIdentifier(NPIdentifier identifier);
        NPObject *RetainObject(NPObject *npobj);
        void ReleaseObject(NPObject *npobj);

    public:
        // These are the static NPP_ functions; NPP_New and NPP_Destroy create and destroy the
        // plugin, the rest are wrappers that dereference NPP->pdata to get at the plugin object
        // and proxy the call to there.
        static NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc,
            char* argn[], char* argv[], NPSavedData* saved);
        static NPError NPP_Destroy(NPP instance, NPSavedData** save);
        static NPError NPP_SetWindow(NPP instance, NPWindow* window);
        static NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
            NPBool seekable, uint16* stype);
        static NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
        static int32 NPP_WriteReady(NPP instance, NPStream* stream);
        static int32 NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len,
            void* buffer);
        static void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
        static void NPP_Print(NPP instance, NPPrint* platformPrint);
        static int16 NPP_HandleEvent(NPP instance, void* event);
        static void NPP_URLNotify(NPP instance, const char* url, NPReason reason,
            void* notifyData);
        static NPError NPP_GetValue(NPP instance, NPPVariable variable, void *ret_alue);
        static NPError NPP_SetValue(NPP instance, NPNVariable variable, void *ret_alue);
    };

}; };

#endif