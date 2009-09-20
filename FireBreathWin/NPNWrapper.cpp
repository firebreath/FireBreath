#include <npapi.h>

/* napapi.h definitions */
void NPN_Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor)
{
	//Looks like some deriving from NPNetscapeFuncs.version is needed.
}

NPError NPN_GetURLNotify(NPP instance, const char* url, const char* target, void* notifyData)
{
	return NPNFuncs.geturlnotify(instance, url, target, notifyData);
}

NPError NPN_GetURL(NPP instance, const char* url, const char* target)
{
	return NPNFuncs.geturl(instance, url, target);
}

NPError NPN_PostURLNotify(NPP instance, const char* url, const char* target, uint32 len,
                          const char* buf, NPBool file, void* notifyData)
{
	return NPNFuncs.posturlnotify(instance, url, target, len, buf, file, notifyData);
}

NPError NPN_PostURL(NPP instance, const char* url, const char* target, uint32 len,
                              const char* buf, NPBool file)
{
	return NPNFuncs.posturl(instance, url, target, len, buf, file);
}

NPError NPN_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
	return NPNFuncs.requestread(stream, rangeList);
}

NPError NPN_NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream)
{
	return NPNFuncs.newstream(instance, type, target, stream);
}

int32 NPN_Write(NPP instance, NPStream* stream, int32 len, void* buffer)
{
	return NPNFuncs.write(instance, stream, len, buffer);
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
	return NPNFuncs.destroystream(instance, stream, reason);
}

void NPN_Status(NPP instance, const char* message)
{
	NPNFuncs.status(instance, message);
}

const char* NPN_UserAgent(NPP instance)
{
	return NPNFuncs.uagent(instance);
}

void* NPN_MemAlloc(uint32 size)
{
	return NPNFuncs.memalloc(size);
}

void NPN_MemFree(void* ptr)
{
	NPNFuncs.memfree(ptr);
}

uint32 NPN_MemFlush(uint32 size)
{
	return NPNFuncs.memflush(size);
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
	NPNFuncs.reloadplugins(reloadPages);
}

#ifdef OJI
JRIEnv* NPN_GetJavaEnv(void)
{
	NPNFuncs.getJavaEnv();
}

jref NPN_GetJavaPeer(NPP instance)
{
	return NPNFuncs.getJavaPeer(instance);
}
#endif

NPError NPN_GetValue(NPP instance, NPNVariable variable, void *value)
{
	return NPNFuncs.getvalue(instance, variable, value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void *value)
{
	return NPNFuncs.setvalue(instance, variable, value);
}

void NPN_InvalidateRect(NPP instance, NPRect *invalidRect)
{
	NPNFuncs.invalidaterect(instance, invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
	NPNFuncs.invalidateregion(instance, invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
	NPNFuncs.forceredraw(instance);
}

void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled)
{
	NPNFuncs.pushpopupsenabledstate(instance, enabled);
}

void NPN_PopPopupsEnabledState(NPP instance)
{
	NPNFuncs.poppopupsenabledstate(instance);
}

void NPN_PluginThreadAsyncCall(NPP instance, void (*func) (void *), void *userData)
{
	NPNFuncs.pluginthreadasynccall(instance, func, userData);
}

/* npruntime.h definitions */
void NPN_ReleaseVariantValue(NPVariant *variant)
{
	NPNFuncs.releasevariantvalue(variant);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name)
{
	return NPNFuncs.getstringidentifier(name);
}

void NPN_GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
	NPNFuncs.getstringidentifiers(names, nameCount, identifiers);
}

NPIdentifier NPN_GetIntIdentifier(int32_t intid)
{
	return NPNFuncs.getintidentifier(intid);
}

bool NPN_IdentifierIsString(NPIdentifier identifier)
{
	return NPNFuncs.identifierisstring(identifier);
}

NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
	return NPNFuncs.utf8fromidentifier(identifier);
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
	return NPNFuncs.intfromidentifier(identifier);
}

NPObject *NPN_CreateObject(NPP npp, NPClass *aClass)
{
	return NPNFuncs.createobject(aClass);
}

NPObject *NPN_RetainObject(NPObject *npobj)
{
	return NPNFuncs.retainobject(npobj);
}

void NPN_ReleaseObject(NPObject *npobj)
{
	NPNFuncs.releaseobject(npobj);
}

bool NPN_Invoke(NPP npp, NPObject *npobj, NPIdentifier methodName, const NPVariant *args, 
								uint32_t argCount, NPVariant *result)
{
	return NPNFuncs.invoke(npp, npobj, methodName, args, argCount, result);
}

bool NPN_InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args,
                       uint32_t argCount, NPVariant *result)
{
	return NPNFuncs.invokeDefault(npp, npobj, args, argCount, result);
}

bool NPN_Evaluate(NPP npp, NPObject *npobj, NPString *script,
                  NPVariant *result)
{
	return NPNFuncs.evaluate(npp, npobj, script, result);
}

bool NPN_GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                     NPVariant *result)
{
	return NPNFuncs.getproperty(npp, npobj, propertyName, result);
}

bool NPN_SetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                     const NPVariant *value)
{
	return NPNFuncs.setproperty(npp, npobj, propertyName, value);
}

bool NPN_RemoveProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
	return NPNFuncs.removeproperty(npp, npobj, propertyName);
}

bool NPN_HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
	return NPNFuncs.hasproperty(npp, npobj, propertyName);
}

bool NPN_HasMethod(NPP npp, NPObject *npobj, NPIdentifier methodName)
{
	return NPNFuncs.hasmethod(npp, npobj, methodName);
}

bool NPN_Enumerate(NPP npp, NPObject *npobj, NPIdentifier **identifier,
                   uint32_t *count)
{
	return NPNFuncs.enumerate(npp, npobj, identifier, count);
}

bool NPN_Construct(NPP npp, NPObject *npobj, const NPVariant *args,
                   uint32_t argCount, NPVariant *result)
{
	return NPNFuncs.construct(npp, npobj, args, argCount, result);
}

void NPN_SetException(NPObject *npobj, const NPUTF8 *message)
{
	NPNFuncs.setexception(npobj, message);
}