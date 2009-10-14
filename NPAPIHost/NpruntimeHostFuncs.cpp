
#include "npupp.h"
#include "npruntime.h"

/* NPN_Invoke */
bool NP_LOADDS NpapiHost_Invoke(NPP npp, NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (obj->_class->invoke) {
		return obj->_class->invoke(obj, methodName, args, argCount, result);
	} else {
		return false;
	}
}

/* NPN_InvokeDefault */
bool NP_LOADDS NpapiHost_InvokeDefault(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (obj->_class->invokeDefault) {
		return obj->_class->invokeDefault(obj, args, argCount, result);
	} else {
		return false;
	}
}

/* NPN_GetProperty */
bool NP_LOADDS NpapiHost_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, NPVariant *result)
{
	if (obj->_class->getProperty) {
		return obj->_class->getProperty(obj, propertyName, result);
	} else {
		return false;
	}
}

/* NPN_SetProperty */
bool NP_LOADDS NpapiHost_SetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, const NPVariant *value)
{
	if (obj->_class->setProperty) {
		return obj->_class->setProperty(obj, propertyName, value);
	} else {
		return false;
	}
}

/* NPN_RemoveProperty */
bool NP_LOADDS NpapiHost_RemoveProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
	if (obj->_class->removeProperty) {
		return obj->_class->removeProperty(obj, propertyName);
	} else {
		return false;
	}
}

/* NPN_HasProperty */
bool NP_LOADDS NpapiHost_HasProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
	if (obj->_class->hasProperty) {
		return obj->_class->hasProperty(obj, propertyName);
	} else {
		return false;
	}
}

/* NPN_HasMethod */
bool NP_LOADDS NpapiHost_HasMethod(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
	if (obj->_class->hasMethod) {
		return obj->_class->hasMethod(obj, propertyName);
	} else {
		return false;
	}
}

/* NPN_Enumerate */
bool NP_LOADDS NpapiHost_Enumerate(NPP npp, NPObject *obj, NPIdentifier **identifier, uint32_t *count)
{
	if (obj->_class->enumerate) {
		return obj->_class->enumerate(obj, identifier, count);
	} else {
		return false;
	}
}

/* NPN_Construct */
bool NP_LOADDS NpapiHost_Construct(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (obj->_class->construct) {
		return obj->_class->construct(obj, args, argCount, result);
	} else {
		return false;
	}
}
