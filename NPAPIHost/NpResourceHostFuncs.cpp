
#include "npupp.h"
#include "npruntime.h"

#include <map>
#include <vector>
#include <string>

typedef std::map<void*, std::string> NpIdStringMap;
typedef std::map<std::string, void*> NpStringIdMap;
typedef std::map<void*, int32_t> NpIdIntMap;
typedef std::map<int32_t, void*> NpIntIdMap;

static unsigned long nextId = 1;
static NpIdStringMap _idStringMap;
static NpStringIdMap _stringIdMap;
static NpIdIntMap _idIntMap;
static NpIntIdMap _intIdMap;

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
		NpapiHost_MemFree((void*)variant->value.stringValue.utf8characters);
    } else if (variant->type == NPVariantType_Object) {
        NpapiHost_ReleaseObject(variant->value.objectValue);
    }
}



/* NPN_GetStringIdentifier */
NPIdentifier NP_LOADDS NpapiHost_GetStringIdentifier(const NPUTF8* name)
{
    std::string sName(name);
    NpStringIdMap::iterator it = _stringIdMap.find(sName);

    if (it != _stringIdMap.end()) {
        return it->second;
    } else {
        void *newId = (void*)nextId++;
        _stringIdMap[sName] = newId;
        _idStringMap[newId] = sName;
        return newId;
    }
}

/* NPN_GetStringIdentifiers */
void NP_LOADDS NpapiHost_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
{
    for (int32_t i = 0; i < nameCount; i++) {
        identifiers[i] = NpapiHost_GetStringIdentifier(names[i]);
    }
}

/* NPN_GetIntIdentifier */
NPIdentifier NP_LOADDS NpapiHost_GetIntIdentifier(int32_t intid)
{
    NpIntIdMap::iterator it = _intIdMap.find(intid);

    if (it != _stringIdMap.end()) {
        return it->second;
    } else {
        void *newId = (void*)nextId++;
        _idIntMap[newId] = intid;
        _intIdMap[intid] = newId;
        return newId;
    }
}

/* NPN_IdentifierIsString */
bool NP_LOADDS NpapiHost_IdentifierIsString(NPIdentifier identifier)
{
	return _idStringMap.find(identifier) != _idStringMap.end();
}

/* NPN_UTF8FromIdentifier */
NPUTF8* NP_LOADDS NpapiHost_UTF8FromIdentifier(NPIdentifier identifier)
{
	NpIdStringMap::iterator it = _idStringMap.find(identifier);

	if (it == _idStringMap.end()) {
		return NULL;
	} else {
		std::string str = it->second;

		NPUTF8* outStr = NpapiHost_MemAlloc(str.size() + 1);
		memcpy(outStr, str.c_str(), str.size() + 1);
		return outStr;
	}
}

/* NPN_IntFromIdentifier */
int32_t NP_LOADDS NpapiHost_IntFromIdentifier(NPIdentifier identifier)
{
	NpIdIntMap::iterator it = _idIntMap.find(identifier);

	if (it == _idIntMap.end()) {
		return 0;
	} else {
		return it->second;
	}
}