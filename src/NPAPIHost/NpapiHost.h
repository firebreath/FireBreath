/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

The implementation for this file is split into 4 files:
    NpapiHost.cpp - this contains the main logic for dealing
                    with plugins

    NpruntimeHostFuncs.cpp
    NpBrowserHostFuncs.cpp
    NpruntimeHostFuncs.cpp
        - These files contain the NPN_ functions that fill
          out the NPNetscapeFuncs struct on plugin startup

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPAPIHOST
#define H_NPAPIHOST

#include "NpapiTypes.h"
#include "npruntime.h"
#include "NpapiTypes.h"
#include "TypeIDMap.h"

namespace FB { namespace Npapi {

    class NpapiHost
    {
    public:
        NpapiHost(NPInitFuncPtr, NPShutdownFuncPtr, NPGetEntryPointsFuncPtr);
        ~NpapiHost();

    public:
        NPNetscapeFuncs *getBrowserFuncs();
        NPP getPluginInstance();

    protected:
        NPP_t m_instance;
        NPNetscapeFuncs m_funcs;
        static FB::TypeIDMap<NPIdentifier> m_idMapper;

        // References to the entry point functions of a plugin
        NPInitFuncPtr init;
        NPShutdownFuncPtr shutdown;
        NPGetEntryPointsFuncPtr getEntryPoints;

    public:
        /* NpruntimeHostFuncs */
        static bool NP_LOADDS NH_Invoke(NPP npp, NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
        static bool NP_LOADDS NH_InvokeDefault(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result);
        static bool NP_LOADDS NH_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, NPVariant *result);
        static bool NP_LOADDS NH_SetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, const NPVariant *value);
        static bool NP_LOADDS NH_RemoveProperty(NPP npp, NPObject *obj, NPIdentifier propertyName);
        static bool NP_LOADDS NH_HasProperty(NPP npp, NPObject *obj, NPIdentifier propertyName);
        static bool NP_LOADDS NH_HasMethod(NPP npp, NPObject *obj, NPIdentifier propertyName);
        static bool NP_LOADDS NH_Enumerate(NPP npp, NPObject *obj, NPIdentifier **identifier, uint32_t *count);
        static bool NP_LOADDS NH_Construct(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result);

    public:
        // NpBrowserHostFuncs
        static NPError  NP_LOADDS NH_GetValue(NPP instance, NPNVariable variable, void *ret_value);
        static NPError  NP_LOADDS NH_SetValue(NPP instance, NPPVariable variable, void *ret_alue);
        static NPError  NP_LOADDS NPN_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData);
        static NPError  NP_LOADDS NH_PostURLNotify(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file, void* notifyData);
        static NPError  NP_LOADDS NH_GetURL(NPP instance, const char* url, const char* window);
        static NPError  NP_LOADDS NH_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData);
        static NPError  NP_LOADDS NH_PostURL(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file);
        static NPError  NP_LOADDS NH_RequestRead(NPStream* stream, NPByteRange* rangeList);
        static NPError  NP_LOADDS NH_NewStream(NPP instance, NPMIMEType type, const char* window, NPStream** stream);
        static int32_t    NP_LOADDS NH_Write(NPP instance, NPStream* stream, int32_t len, void* buffer);
        static NPError  NP_LOADDS NH_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
        static void     NP_LOADDS NH_Status(NPP instance, const char* message);
        static const char* NP_LOADDS NH_UserAgent(NPP instance);
        static void     NP_LOADDS NH_ReloadPlugins(NPBool reloadPages);
        static void     NP_LOADDS NH_InvalidateRect(NPP instance, NPRect *rect);
        static void     NP_LOADDS NH_InvalidateRegion(NPP instance, NPRegion region);
        static void     NP_LOADDS NH_ForceRedraw(NPP instance);
        static void     NP_LOADDS NH_PushPopupsEnabledState(NPP npp, NPBool enabled);
        static void     NP_LOADDS NH_PopPopupsEnabledState(NPP npp);
        static void     NP_LOADDS NH_PluginThreadAsyncCall(NPP instance, void (*func)(void *), void *userData);
        static bool     NP_LOADDS NH_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result);
        static void     NP_LOADDS NH_SetException(NPObject *obj, const NPUTF8 *message);
        static uint32_t NP_LOADDS NH_ScheduleTimer(NPP npp, uint32_t interval, NPBool repeat, 
                                                   void (*timerFunc)(NPP npp, uint32_t timerID));
        static void     NP_LOADDS NH_UnscheduleTimer(NPP npp, uint32_t timerID);

    public:
        // NpResourceHostFuncs
        static void*    NP_LOADDS NH_MemAlloc(uint32_t size);
        static void     NP_LOADDS NH_MemFree(void* ptr);
        static uint32_t   NP_LOADDS NH_MemFlush(uint32_t size);
        static NPObject* NP_LOADDS NH_CreateObject(NPP npp, NPClass *aClass);
        static NPObject* NP_LOADDS NH_RetainObject(NPObject *obj);
        static void     NP_LOADDS NH_ReleaseObject(NPObject *obj);
        static void     NP_LOADDS NH_ReleaseVariantValue(NPVariant *variant);
        static NPIdentifier NP_LOADDS NH_GetStringIdentifier(const NPUTF8* name);
        static void     NP_LOADDS NH_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers);
        static NPIdentifier NP_LOADDS NH_GetIntIdentifier(int32_t intid);
        static bool     NP_LOADDS NH_IdentifierIsString(NPIdentifier identifier);
        static NPUTF8*  NP_LOADDS NH_UTF8FromIdentifier(NPIdentifier identifier);
        static int32_t  NP_LOADDS NH_IntFromIdentifier(NPIdentifier identifier);
    };

}; };

#endif

