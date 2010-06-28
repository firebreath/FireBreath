/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPAPIBROWSERHOST
#define H_NPAPIBROWSERHOST

#include "NpapiTypes.h"
#include "BrowserHostWrapper.h"

namespace FB { namespace Npapi {

    class NpapiPluginModule;
    class NPObjectAPI;

    class NpapiBrowserHost :
        public FB::BrowserHostWrapper
    {
    public:
        NpapiBrowserHost(NpapiPluginModule *module, NPP npp);
        virtual ~NpapiBrowserHost(void);

        void setBrowserFuncs(NPNetscapeFuncs *pFuncs);

    public:
        virtual BrowserStream* createStream(const std::string& url, PluginEventSink* callback, 
                                            bool cache = true, bool seekable = false, 
                                            size_t internalBufferSize = 128 * 1024 );

    public:
        virtual void ScheduleAsyncCall(void (*func)(void *), void *userData);
        virtual void *getContextID() { return (void *)m_npp; }

    public:
        FB::JSAPI_DOMDocument getDOMDocument();
        FB::JSAPI_DOMWindow getDOMWindow();
        void evaluateJavaScript(const std::string &script);

    public:
        FB::variant getVariant(const NPVariant *npVar);
        void getNPVariant(NPVariant *dst, const FB::variant &var);

    // NPN_ functions -- for scope reasons, we no longer access these using the global functions
    protected:
        NPNetscapeFuncs NPNFuncs;   // Function pointers
        NpapiPluginModule *module;
        NPP m_npp;
        FB::AutoPtr<NPObjectAPI> m_htmlDoc;
        FB::AutoPtr<NPObjectAPI> m_htmlWin;

    public:
        /* These are proxied to the module */
        void* MemAlloc(uint32_t size);
        void MemFree(void* ptr);
        uint32_t MemFlush(uint32_t size);

        NPObject *RetainObject(NPObject *npobj);
        void ReleaseObject(NPObject *npobj);
        void ReleaseVariantValue(NPVariant *variant);

        NPIdentifier GetStringIdentifier(const NPUTF8 *name);
        void GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers);
        NPIdentifier GetIntIdentifier(int32_t intid);
        bool IdentifierIsString(NPIdentifier identifier);
        NPUTF8 *UTF8FromIdentifier(NPIdentifier identifier);
        std::string StringFromIdentifier(NPIdentifier identifier);
        int32_t IntFromIdentifier(NPIdentifier identifier);

        /* npapi.h definitions */
        NPError GetURLNotify(const char* url, const char* target, void* notifyData);
        NPError GetURL(const char* url, const char* target);
        NPError PostURLNotify(const char* url, const char* target, uint32_t len, const char* buf, NPBool file, void* notifyData);
        NPError PostURL(const char* url, const char* target, uint32_t len, const char* buf, NPBool file);
        NPError RequestRead(NPStream* stream, NPByteRange* rangeList);
        NPError NewStream(NPMIMEType type, const char* target, NPStream** stream);
        int32_t Write(NPStream* stream, int32_t len, void* buffer);
        NPError DestroyStream(NPStream* stream, NPReason reason);
        void SetStatus(const char* message);
        const char* UserAgent();
        NPError GetValue(NPNVariable variable, void *value);
        NPError SetValue(NPPVariable variable, void *value);
        void InvalidateRect(NPRect *invalidRect);
        void InvalidateRegion(NPRegion invalidRegion);
        void ForceRedraw();
        void PushPopupsEnabledState(NPBool enabled);
        void PopPopupsEnabledState();
        void PluginThreadAsyncCall(void (*func) (void *), void *userData);
        /* npruntime.h definitions */
        NPObject *CreateObject(NPClass *aClass);
        bool Invoke(NPObject *npobj, NPIdentifier methodName, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result);
        bool InvokeDefault(NPObject *npobj, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result);
        bool Evaluate(NPObject *npobj, NPString *script,
                                        NPVariant *result);
        bool GetProperty(NPObject *npobj, NPIdentifier propertyName,
                                        NPVariant *result);
        bool SetProperty(NPObject *npobj, NPIdentifier propertyName,
                                        const NPVariant *value);
        bool RemoveProperty(NPObject *npobj, NPIdentifier propertyName);
        bool HasProperty(NPObject *npobj, NPIdentifier propertyName);
        bool HasMethod(NPObject *npobj, NPIdentifier methodName);
        bool Enumerate(NPObject *npobj, NPIdentifier **identifier,
                                        uint32_t *count);
        bool Construct(NPObject *npobj, const NPVariant *args,
                                        uint32_t argCount, NPVariant *result);
        void SetException(NPObject *npobj, const NPUTF8 *message);
    };
}; };

#endif
