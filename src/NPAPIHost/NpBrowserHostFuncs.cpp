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

#include "NpapiHost.h"

/*
 *  Netscape entry points
 */

using namespace FB::Npapi;

/* NPN_GetValue */
NPError NP_LOADDS NpapiHost::NH_GetValue(NPP instance, NPNVariable variable, void *ret_value)
{
    // TODO: Implement NpapiHost_GetValue
    return NPERR_INVALID_PARAM;
}

/* NPN_SetValue */
NPError NP_LOADDS NpapiHost::NH_SetValue(NPP instance, NPPVariable variable, void *ret_alue)
{
    // TODO: Implement NpapiHost_SetValue
    return NPERR_INVALID_PARAM;
}


/* NPN_GetUrlNotify */
NPError NP_LOADDS NPN_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_PostUrlNotify */
NPError NP_LOADDS NpapiHost::NH_PostURLNotify(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_GetUrl */
NPError NP_LOADDS NpapiHost::NH_GetURL(NPP instance, const char* url, const char* window)
{
    return NPERR_INVALID_PARAM;
}

NPError NP_LOADDS NpapiHost::NH_GetURLNotify(NPP instance, const char* url, const char* window, void* notifyData)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_PostUrl */
NPError NP_LOADDS NpapiHost::NH_PostURL(NPP instance, const char* url, const char* window, uint32_t len, const char* buf, NPBool file)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_RequestRead */
NPError NP_LOADDS NpapiHost::NH_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_NewStream */
NPError NP_LOADDS NpapiHost::NH_NewStream(NPP instance, NPMIMEType type, const char* window, NPStream** stream)
{
    return NPERR_INVALID_PARAM;
}

/* NPN_Write */
int32_t NP_LOADDS NpapiHost::NH_Write(NPP instance, NPStream* stream, int32_t len, void* buffer)
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
const char* NP_LOADDS NpapiHost::NH_UserAgent(NPP instance)
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
void NP_LOADDS NpapiHost::NH_PushPopupsEnabledState(NPP npp, NPBool enabled)
{
    
}

/* NPN_PopPopupsEnabledState */
void NP_LOADDS NpapiHost::NH_PopPopupsEnabledState(NPP npp)
{
    
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

/* NPN_ScheduleTimer */
uint32_t NP_LOADDS NpapiHost::NH_ScheduleTimer(NPP npp, uint32_t interval, NPBool repeat, 
                                void (*timerFunc)(NPP npp, uint32_t timerID)) {
    return 0;
}

/* NPN_UnscheduleTimer */
void NP_LOADDS NpapiHost::NH_UnscheduleTimer(NPP npp, uint32_t timerID) {

}

