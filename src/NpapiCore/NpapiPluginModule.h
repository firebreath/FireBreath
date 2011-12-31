/**********************************************************\
Original Authors: Richard Bateman (taxilian)
Don Jordan (kc7zax)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPAPIPLUGINMODULE
#define H_NPAPIPLUGINMODULE

#include "APITypes.h"
#include "NpapiTypes.h"
#include "FactoryBase.h"
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

namespace FB {
    namespace Npapi {
        FB_FORWARD_PTR(NpapiPlugin);
        // Get instance of NpapiPlugin from an NPP instance
        NpapiPluginPtr getPlugin(NPP instance);

        class NpapiPluginModule : boost::noncopyable
        {
        public:
            NpapiPluginModule();
            virtual ~NpapiPluginModule();

            typedef std::map<const void*,NpapiPluginModule*> Modules;
            static NpapiPluginModule* GetModule(const void* key);
            static void ReleaseModule(const void*);


            void setNetscapeFuncs(NPNetscapeFuncs *npnFuncs);
            void getPluginFuncs(NPPluginFuncs* pFuncs);

            NPNetscapeFuncs NPNFuncs;

        protected:
            boost::thread::id m_threadId;
            static volatile uint32_t PluginModuleInitialized;
            static Modules m_modules;


        public:
            void assertMainThread();

        public:
            // These are the NPN_ functions that don't require a NPP
            void Version(int* plugin_major, int* plugin_minor, int* netscape_major,
                int* netscape_minor);
            void* MemAlloc(uint32_t size);
            void MemFree(void* ptr);
            uint32_t MemFlush(uint32_t size);
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

#ifdef FB_MACOSX
            // This is used on Mac OS X when we don't have pluginthreadasynccall
            static void scheduleAsyncCallback(NPP npp, void (*func)(void *), void *userData);
#endif
        public:
            // These are the static NPP_ functions; NPP_New and NPP_Destroy create and destroy the
            // plugin, the rest are wrappers that dereference NPP->pdata to get at the plugin object
            // and proxy the call to there.
            static NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
                char* argn[], char* argv[], NPSavedData* saved);
            static NPError NPP_Destroy(NPP instance, NPSavedData** save);
            static NPError NPP_SetWindow(NPP instance, NPWindow* window);
            static NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
                NPBool seekable, uint16_t* stype);
            static NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
            static int32_t NPP_WriteReady(NPP instance, NPStream* stream);
            static int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len,
                void* buffer);
            static void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
            static void NPP_Print(NPP instance, NPPrint* platformPrint);
            static int16_t NPP_HandleEvent(NPP instance, void* event);
            static void NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                void* notifyData);
            static NPError NPP_GetValue(NPP instance, NPPVariable variable, void *ret_alue);
            static NPError NPP_SetValue(NPP instance, NPNVariable variable, void *ret_alue);
        };
    };
};

#endif

