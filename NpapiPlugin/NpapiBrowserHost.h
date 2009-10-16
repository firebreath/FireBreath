/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPAPIBROWSERHOST
#define H_NPAPIBROWSERHOST

#include "npupp.h"
#include "BrowserHostWrapper.h"

namespace FB
{
namespace Npapi
{
    class NpapiBrowserHost :
        public FB::BrowserHostWrapper
    {
    public:
        NpapiBrowserHost(void);
        virtual ~NpapiBrowserHost(void);

        void setBrowserFuncs(NPNetscapeFuncs *pFuncs);

    // NPN_ functions -- for scope reasons, we no longer access these using the global functions
    protected:
        NPNetscapeFuncs NPNFuncs;   // Function pointers
    public:
        /* npapi.h definitions */
        void Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor);
        NPError GetURLNotify(NPP instance, const char* url, const char* target, void* notifyData);
        NPError GetURL(NPP instance, const char* url, const char* target);
        NPError PostURLNotify(NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file, void* notifyData);
        NPError PostURL(NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file);
        NPError RequestRead(NPStream* stream, NPByteRange* rangeList);
        NPError NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream);
        int32 Write(NPP instance, NPStream* stream, int32 len, void* buffer);
        NPError DestroyStream(NPP instance, NPStream* stream, NPReason reason);
        void Status(NPP instance, const char* message);
        const char* UserAgent(NPP instance);
        void* MemAlloc(uint32 size);
        void MemFree(void* ptr);
        uint32 MemFlush(uint32 size);
        void ReloadPlugins(NPBool reloadPages);
        NPError GetValue(NPP instance, NPNVariable variable, void *value);
        NPError SetValue(NPP instance, NPPVariable variable, void *value);
        void InvalidateRect(NPP instance, NPRect *invalidRect);
        void InvalidateRegion(NPP instance, NPRegion invalidRegion);
        void ForceRedraw(NPP instance);
        void PushPopupsEnabledState(NPP instance, NPBool enabled);
        void PopPopupsEnabledState(NPP instance);
        void PluginThreadAsyncCall(NPP instance, void (*func) (void *), void *userData);
        /* npruntime.h definitions */
        void ReleaseVariantValue(NPVariant *variant);
        NPIdentifier GetStringIdentifier(const NPUTF8 *name);
        void GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers);
        NPIdentifier GetIntIdentifier(int32_t intid);
        bool IdentifierIsString(NPIdentifier identifier);
        NPUTF8 *UTF8FromIdentifier(NPIdentifier identifier);
        int32_t IntFromIdentifier(NPIdentifier identifier);
        NPObject *CreateObject(NPP npp, NPClass *aClass);
        NPObject *RetainObject(NPObject *npobj);
        void ReleaseObject(NPObject *npobj);
        bool Invoke(NPP npp, NPObject *npobj, NPIdentifier methodName, const NPVariant *args, 
                                        uint32_t argCount, NPVariant *result);
        bool InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result);
        bool Evaluate(NPP npp, NPObject *npobj, NPString *script,
                                        NPVariant *result);
        bool GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                                        NPVariant *result);
        bool SetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                                        const NPVariant *value);
        bool RemoveProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName);
        bool HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName);
        bool HasMethod(NPP npp, NPObject *npobj, NPIdentifier methodName);
        bool Enumerate(NPP npp, NPObject *npobj, NPIdentifier **identifier,
                                        uint32_t *count);
        bool Construct(NPP npp, NPObject *npobj, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result);
        void SetException(NPObject *npobj, const NPUTF8 *message);
    };
};
};

#endif