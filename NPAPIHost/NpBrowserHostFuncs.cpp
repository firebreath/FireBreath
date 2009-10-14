
#include "npupp.h"
#include "npruntime.h"

/*
 *  Netscape entry points
 */


/* NPN_GetValue */
NPError	NP_LOADDS NpapiHost_GetValue(NPP instance, NPNVariable variable, void *ret_value)
{
}

/* NPN_SetValue */
NPError	NP_LOADDS NpapiHost_SetValue(NPP instance, NPPVariable variable, void *ret_alue)
{
}


/* NPN_GetUrlNotify */
NPError	NP_LOADDS NPN_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData)
{
}

/* NPN_PostUrlNotify */
NPError NP_LOADDS NpapiHost_PostURLNotify(NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file, void* notifyData)
{
}

/* NPN_GetUrl */
NPError	NP_LOADDS NpapiHost_GetURL(NPP instance, const char* url, const char* window)
{
}

/* NPN_PostUrl */
NPError NP_LOADDS NpapiHost_PostURL(NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file)
{
}

/* NPN_RequestRead */
NPError	NP_LOADDS NpapiHost_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
}

/* NPN_NewStream */
NPError	NP_LOADDS NpapiHost_NewStream(NPP instance, NPMIMEType type, const char* window, NPStream** stream)
{
}

/* NPN_Write */
int32 NP_LOADDS NpapiHost_Write(NPP instance, NPStream* stream, int32 len, void* buffer)
{
}

/* NPN_DestroyStream */
NPError NP_LOADDS NpapiHost_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
}

/* NPN_Status */
void NP_LOADDS NpapiHost_Status(NPP instance, const char* message)
{
}

/* NPN_UserAgent */
const char*	NP_LOADDS NpapiHost_UserAgent(NPP instance)
{
}

/* NPN_ReloadPlugins */
void NP_LOADDS NpapiHost_ReloadPlugins(NPBool reloadPages)
{
}

/* NPN_GetJavaEnv */
JRIEnv* NP_LOADDS NpapiHost_GetJavaEnv(void)
{
}

/* NPN_GetJavaPeer */
jref NP_LOADDS NpapiHost_GetJavaPeer(NPP instance)
{
}

/* NPN_InvalidateRect */
void NP_LOADDS NpapiHost_InvalidateRect(NPP instance, NPRect *rect)
{
}

/* NPN_InvalidateRegion */
void NP_LOADDS NpapiHost_InvalidateRegion(NPP instance, NPRegion region)
{
}

/* NPN_ForceRedraw */
void NP_LOADDS NpapiHost_ForceRedraw(NPP instance)
{
}

/* NPN_PushPopupsEnabledStateUPP */
bool NP_LOADDS NpapiHost_PushPopupsEnabledState(NPP npp, NPBool enabled)
{
}

/* NPN_PopPopupsEnabledState */
bool NP_LOADDS NpapiHost_PopPopupsEnabledState(NPP npp)
{
}

/* NPN_PluginThreadAsyncCall */
void NP_LOADDS NpapiHost_PluginThreadAsyncCall(NPP instance, void (*func)(void *), void *userData)
{
}

/* NPN_Evaluate */
bool NP_LOADDS NpapiHost_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result)
{
}

/* NPN_SetException */
void NP_LOADDS NpapiHost_SetException(NPObject *obj, const NPUTF8 *message)
{
	
}