/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "npupp.h"
#include "npruntime.h"

#include "NpapiHost.h"

/*
 *  Netscape entry points
 */

using namespace FB::Npapi;

/* NPN_GetValue */
NPError	NP_LOADDS NpapiHost::NH_GetValue(NPP instance, NPNVariable variable, void *ret_value)
{
    // TODO: Implement NpapiHost_GetValue
    return NPERR_INVALID_PARAM;
}

/* NPN_SetValue */
NPError	NP_LOADDS NpapiHost::NH_SetValue(NPP instance, NPPVariable variable, void *ret_alue)
{
    // TODO: Implement NpapiHost_SetValue
    return NPERR_INVALID_PARAM;
}


/* NPN_GetUrlNotify */
NPError	NP_LOADDS NPN_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_PostUrlNotify */
NPError NP_LOADDS NpapiHost::NH_PostURLNotify(NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_GetUrl */
NPError	NP_LOADDS NpapiHost::NH_GetURL(NPP instance, const char* url, const char* window)
{
    return NPERR_INVALID_PARAM;
}

NPError NP_LOADDS NpapiHost::NH_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_PostUrl */
NPError NP_LOADDS NpapiHost::NH_PostURL(NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_RequestRead */
NPError	NP_LOADDS NpapiHost::NH_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_NewStream */
NPError	NP_LOADDS NpapiHost::NH_NewStream(NPP instance, NPMIMEType type, const char* window, NPStream** stream)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_Write */
int32 NP_LOADDS NpapiHost::NH_Write(NPP instance, NPStream* stream, int32 len, void* buffer)
{
    return 0;
}

/* NPN_DestroyStream */
NPError NP_LOADDS NpapiHost::NH_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_Status */
void NP_LOADDS NpapiHost::NH_Status(NPP instance, const char* message)
{
}

/* NPN_UserAgent */
const char*	NP_LOADDS NpapiHost::NH_UserAgent(NPP instance)
{
    return "Fake Browser Host!";
}

/* NPN_ReloadPlugins */
void NP_LOADDS NpapiHost::NH_ReloadPlugins(NPBool reloadPages)
{
}

/* NPN_InvalidateRect */
void NP_LOADDS NpapiHost::NH_InvalidateRect(NPP instance, NPRect *rect)
{
}

/* NPN_InvalidateRegion */
void NP_LOADDS NpapiHost::NH_InvalidateRegion(NPP instance, NPRegion region)
{
}

/* NPN_ForceRedraw */
void NP_LOADDS NpapiHost::NH_ForceRedraw(NPP instance)
{
}

/* NPN_PushPopupsEnabledStateUPP */
bool NP_LOADDS NpapiHost::NH_PushPopupsEnabledState(NPP npp, NPBool enabled)
{
    return true;
}

/* NPN_PopPopupsEnabledState */
bool NP_LOADDS NpapiHost::NH_PopPopupsEnabledState(NPP npp)
{
    return true;
}

/* NPN_PluginThreadAsyncCall */
void NP_LOADDS NpapiHost::NH_PluginThreadAsyncCall(NPP instance, void (*func)(void *), void *userData)
{
    func(userData);
}

/* NPN_Evaluate */
bool NP_LOADDS NpapiHost::NH_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result)
{
    return false;
}

/* NPN_SetException */
void NP_LOADDS NpapiHost::NH_SetException(NPObject *obj, const NPUTF8 *message)
{
	
}