
#include "npupp.h"
#include "npruntime.h"

/* NPN_MemAlloc */
void* NP_LOADDS NpapiHost_MemAlloc(uint32 size)
{
	return malloc(size);
}

/* NPN__MemFree */
void NP_LOADDS NpapiHost_MemFree(void* ptr)
{
	free(ptr);
}

/* NPN_MemFlush */
uint32 NP_LOADDS NpapiHost_MemFlush(uint32 size)
{
	return 0;
}

/* NPN_CreateObject */

NPObject* NP_LOADDS NpapiHost_CreateObject(NPP npp, NPClass *aClass)
{
	NPObject *newObj(NULL);
	if (aClass->allocate) {
		newObj = aClass->allocate(npp, aClass);
	} else {
		newObj = (NPObject *)malloc(sizeof(NPObject));
	}
	newObj->referenceCount = 1;
	newObj->_class = aClass;
	return newObj;
}

/* NPN_RetainObject */
NPObject* NP_LOADDS NpapiHost_RetainObject(NPObject *obj)
{
	obj->referenceCount++;
}

/* NPN_ReleaseObject */
void NP_LOADDS NpapiHost_ReleaseObject(NPObject *obj)
{
	if (--(obj->referenceCount) == 0) {
		if (obj->_class->deallocate) {
			obj->_class->deallocate(obj);
		} else {
			free(obj);
		}
	}
}

/* NPN_ReleaseVariantValue */
void NP_LOADDS NpapiHost_ReleaseVariantValue(NPVariant *variant)
{
	if (variant->type == NPVariantType_String) {
		NPN_FreeMem(variant->value.stringValue.utf8characters);
	} else if (variant->type == NPVariantType_Object) {
		NPN_ReleaseObject(variant->value.objectValue);
	}
}

/* NPN_GetStringIdentifier */
NPIdentifier NP_LOADDS NpapiHost_GetStringIdentifier(const NPUTF8* name)
{
}

/* NPN_GetStringIdentifiers */
void NP_LOADDS NpapiHost_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
{
}

/* NPN_GetIntIdentifier */
NPIdentifier NP_LOADDS NpapiHost_GetIntIdentifier(int32_t intid)
{
}

/* NPN_IdentifierIsString */
bool NP_LOADDS NpapiHost_IdentifierIsString(NPIdentifier identifier)
{
}

/* NPN_UTF8FromIdentifier */
NPUTF8* NP_LOADDS NpapiHost_UTF8FromIdentifier(NPIdentifier identifier)
{
}

/* NPN_IntFromIdentifier */
int32_t NP_LOADDS NpapiHost_IntFromIdentifier(NPIdentifier identifier)
{
}