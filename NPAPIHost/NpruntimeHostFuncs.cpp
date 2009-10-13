
#include "npupp.h"
#include "npruntime.h"

/* NPN_Invoke */
bool NP_LOADDS NpapiHost_Invoke(NPP npp, NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
}

/* NPN_InvokeDefault */
bool NP_LOADDS NpapiHost_InvokeDefault(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
}

/* NPN_Evaluate */
bool NP_LOADDS NpapiHost_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result)
{
}

/* NPN_GetProperty */
bool NP_LOADDS NpapiHost_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, NPVariant *result)
{
}

/* NPN_SetProperty */
bool NP_LOADDS NpapiHost_SetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, const NPVariant *value)
{
}

/* NPN_RemoveProperty */
bool NP_LOADDS NpapiHost_RemoveProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
}

/* NPN_HasProperty */
bool NP_LOADDS NpapiHost_HasProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
}

/* NPN_HasMethod */
bool NP_LOADDS NpapiHost_HasMethod(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
}

/* NPN_SetException */
void NP_LOADDS NpapiHost_SetException(NPObject *obj, const NPUTF8 *message)
{
}

/* NPN_Enumerate */
bool NP_LOADDS NpapiHost_Enumerate(NPP npp, NPObject *obj, NPIdentifier **identifier, uint32_t *count)
{
}

/* NPN_Construct */
bool NP_LOADDS NpapiHost_Construct(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
}