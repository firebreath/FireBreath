/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <memory.h>

#include "NpapiBrowserHost.h"

using namespace FB::Npapi;

NpapiBrowserHost::NpapiBrowserHost(void)
{
    // Initialize NPNFuncs to NULL values
    memset(&NPNFuncs, 0, sizeof(NPNetscapeFuncs));
}

NpapiBrowserHost::~NpapiBrowserHost(void)
{
}

void NpapiBrowserHost::setBrowserFuncs(NPNetscapeFuncs *pFuncs)
{
    NPNFuncs.size = pFuncs->size;
    NPNFuncs.version = pFuncs->version;
    NPNFuncs.geturl = pFuncs->geturl;
    NPNFuncs.posturl = pFuncs->posturl;
    NPNFuncs.requestread = pFuncs->requestread;
    NPNFuncs.newstream = pFuncs->newstream;
    NPNFuncs.write = pFuncs->write;
    NPNFuncs.destroystream = pFuncs->destroystream;
    NPNFuncs.status = pFuncs->status;
    NPNFuncs.uagent = pFuncs->uagent;
    NPNFuncs.memalloc = pFuncs->memalloc;
    NPNFuncs.memfree = pFuncs->memfree;
    NPNFuncs.memflush = pFuncs->memflush;
    NPNFuncs.reloadplugins = pFuncs->reloadplugins;
    NPNFuncs.geturlnotify = pFuncs->geturlnotify;
    NPNFuncs.posturlnotify = pFuncs->posturlnotify;
    NPNFuncs.getvalue = pFuncs->getvalue;
    NPNFuncs.setvalue = pFuncs->setvalue;
    NPNFuncs.invalidaterect = pFuncs->invalidaterect;
    NPNFuncs.invalidateregion = pFuncs->invalidateregion;
    NPNFuncs.forceredraw = pFuncs->forceredraw;
    NPNFuncs.getstringidentifier = pFuncs->getstringidentifier;
    NPNFuncs.getstringidentifiers = pFuncs->getstringidentifiers;
    NPNFuncs.getintidentifier = pFuncs->getintidentifier;
    NPNFuncs.identifierisstring = pFuncs->identifierisstring;
    NPNFuncs.utf8fromidentifier = pFuncs->utf8fromidentifier;
    NPNFuncs.intfromidentifier = pFuncs->intfromidentifier;
    NPNFuncs.createobject = pFuncs->createobject;
    NPNFuncs.retainobject = pFuncs->retainobject;
    NPNFuncs.releaseobject = pFuncs->releaseobject;
    NPNFuncs.invoke = pFuncs->invoke;
    NPNFuncs.invokeDefault = pFuncs->invokeDefault;
    NPNFuncs.evaluate = pFuncs->evaluate;
    NPNFuncs.getproperty = pFuncs->getproperty;
    NPNFuncs.setproperty = pFuncs->setproperty;
    NPNFuncs.removeproperty = pFuncs->removeproperty;
    NPNFuncs.hasproperty = pFuncs->hasproperty;
    NPNFuncs.hasmethod = pFuncs->hasmethod;
    NPNFuncs.releasevariantvalue = pFuncs->releasevariantvalue;
    NPNFuncs.setexception = pFuncs->setexception;
    NPNFuncs.pushpopupsenabledstate = pFuncs->pushpopupsenabledstate;
    NPNFuncs.poppopupsenabledstate = pFuncs->poppopupsenabledstate;
    NPNFuncs.enumerate = pFuncs->enumerate;
    NPNFuncs.pluginthreadasynccall = pFuncs->pluginthreadasynccall;
    NPNFuncs.construct = pFuncs->construct;
}

/* npapi.h definitions */
void NpapiBrowserHost::Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor)
{
    //Looks like some deriving from NPNetscapeFuncs.version is needed.
}

NPError NpapiBrowserHost::GetURLNotify(NPP instance, const char* url, const char* target, void* notifyData)
{
    if (NPNFuncs.geturlnotify != NULL) {
        return NPNFuncs.geturlnotify(instance, url, target, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::GetURL(NPP instance, const char* url, const char* target)
{
    if (NPNFuncs.geturl != NULL) {
        return NPNFuncs.geturl(instance, url, target);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURLNotify(NPP instance, const char* url, const char* target, uint32 len,
                                        const char* buf, NPBool file, void* notifyData)
{
    if (NPNFuncs.posturlnotify != NULL) {
        return NPNFuncs.posturlnotify(instance, url, target, len, buf, file, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURL(NPP instance, const char* url, const char* target, uint32 len,
                                  const char* buf, NPBool file)
{
    if (NPNFuncs.posturl != NULL) {
        return NPNFuncs.posturl(instance, url, target, len, buf, file);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::RequestRead(NPStream* stream, NPByteRange* rangeList)
{
    if (NPNFuncs.requestread != NULL) {
        return NPNFuncs.requestread(stream, rangeList);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream)
{
    if (NPNFuncs.newstream != NULL) {
        return NPNFuncs.newstream(instance, type, target, stream);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32 NpapiBrowserHost::Write(NPP instance, NPStream* stream, int32 len, void* buffer)
{
    if (NPNFuncs.write != NULL) {
        return NPNFuncs.write(instance, stream, len, buffer);
    } else {
        return 0;
    }
}

NPError NpapiBrowserHost::DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    if (NPNFuncs.destroystream != NULL) {
        return NPNFuncs.destroystream(instance, stream, reason);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void NpapiBrowserHost::Status(NPP instance, const char* message)
{
    if (NPNFuncs.status != NULL) {
        NPNFuncs.status(instance, message);
    }
}

const char* NpapiBrowserHost::UserAgent(NPP instance)
{
    if (NPNFuncs.uagent != NULL) {
        return NPNFuncs.uagent(instance);
    } else {
        return NULL;
    }
}

void* NpapiBrowserHost::MemAlloc(uint32 size)
{
    if (NPNFuncs.memalloc != NULL) {
        return NPNFuncs.memalloc(size);
    } else {
        return NULL;
    }
}

void NpapiBrowserHost::MemFree(void* ptr)
{
    if (NPNFuncs.memfree != NULL) {
        NPNFuncs.memfree(ptr);
    }
}

uint32 NpapiBrowserHost::MemFlush(uint32 size)
{
    if (NPNFuncs.memflush != NULL) {
        return NPNFuncs.memflush(size);
    } else {
        return 0;
    }
}

void NpapiBrowserHost::ReloadPlugins(NPBool reloadPages)
{
    if (NPNFuncs.reloadplugins != NULL) {
        NPNFuncs.reloadplugins(reloadPages);
    }
}

NPError NpapiBrowserHost::GetValue(NPP instance, NPNVariable variable, void *value)
{
    if (NPNFuncs.getvalue != NULL) {
        return NPNFuncs.getvalue(instance, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::SetValue(NPP instance, NPPVariable variable, void *value)
{
    if (NPNFuncs.setvalue != NULL) {
        return NPNFuncs.setvalue(instance, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void NpapiBrowserHost::InvalidateRect(NPP instance, NPRect *invalidRect)
{
    if (NPNFuncs.invalidaterect != NULL) {
        NPNFuncs.invalidaterect(instance, invalidRect);
    }
}

void NpapiBrowserHost::InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
    if (NPNFuncs.invalidateregion != NULL) {
        NPNFuncs.invalidateregion(instance, invalidRegion);
    }
}

void NpapiBrowserHost::ForceRedraw(NPP instance)
{
    if (NPNFuncs.forceredraw != NULL) {
        NPNFuncs.forceredraw(instance);
    }
}

void NpapiBrowserHost::PushPopupsEnabledState(NPP instance, NPBool enabled)
{
    if (NPNFuncs.pushpopupsenabledstate != NULL) {
        NPNFuncs.pushpopupsenabledstate(instance, enabled);
    }
}

void NpapiBrowserHost::PopPopupsEnabledState(NPP instance)
{
    if (NPNFuncs.poppopupsenabledstate != NULL) {
        NPNFuncs.poppopupsenabledstate(instance);
    }
}

void NpapiBrowserHost::PluginThreadAsyncCall(NPP instance, void (*func) (void *), void *userData)
{
    if (NPNFuncs.pluginthreadasynccall != NULL) {
        NPNFuncs.pluginthreadasynccall(instance, func, userData);
    }
}

/* npruntime.h definitions */
void NpapiBrowserHost::ReleaseVariantValue(NPVariant *variant)
{
    if (NPNFuncs.releasevariantvalue != NULL) {
        NPNFuncs.releasevariantvalue(variant);
    }
}

NPIdentifier NpapiBrowserHost::GetStringIdentifier(const NPUTF8 *name)
{
    if (NPNFuncs.getstringidentifier != NULL) {
        return NPNFuncs.getstringidentifier(name);
    } else {
        return NULL;
    }
}

void NpapiBrowserHost::GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
    if (NPNFuncs.getstringidentifiers != NULL) {
        NPNFuncs.getstringidentifiers(names, nameCount, identifiers);
    }
}

NPIdentifier NpapiBrowserHost::GetIntIdentifier(int32_t intid)
{
    if (NPNFuncs.getintidentifier != NULL) {
        return NPNFuncs.getintidentifier(intid);
    } else {
        return NULL;
    }
}

bool NpapiBrowserHost::IdentifierIsString(NPIdentifier identifier)
{
    if (NPNFuncs.identifierisstring != NULL) {
        return NPNFuncs.identifierisstring(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPUTF8 *NpapiBrowserHost::UTF8FromIdentifier(NPIdentifier identifier)
{
    if (NPNFuncs.utf8fromidentifier != NULL) {
        return NPNFuncs.utf8fromidentifier(identifier);
    } else {
        return NULL;
    }
}

int32_t NpapiBrowserHost::IntFromIdentifier(NPIdentifier identifier)
{
    if (NPNFuncs.intfromidentifier != NULL) {
        return NPNFuncs.intfromidentifier(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPObject *NpapiBrowserHost::CreateObject(NPP npp, NPClass *aClass)
{
    if (NPNFuncs.createobject != NULL) {
        return NPNFuncs.createobject(npp, aClass);
    } else {
        return NULL;
    }
}

NPObject *NpapiBrowserHost::RetainObject(NPObject *npobj)
{
    if (NPNFuncs.retainobject != NULL) {
        return NPNFuncs.retainobject(npobj);
    } else {
        return npobj;
    }
}

void NpapiBrowserHost::ReleaseObject(NPObject *npobj)
{
    if (NPNFuncs.releaseobject != NULL) {
        NPNFuncs.releaseobject(npobj);
    }
}

bool NpapiBrowserHost::Invoke(NPP npp, NPObject *npobj, NPIdentifier methodName, const NPVariant *args, 
                              uint32_t argCount, NPVariant *result)
{
    if (NPNFuncs.invoke != NULL) {
        return NPNFuncs.invoke(npp, npobj, methodName, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args,
                                     uint32_t argCount, NPVariant *result)
{
    if (NPNFuncs.invokeDefault != NULL) {
        return NPNFuncs.invokeDefault(npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Evaluate(NPP npp, NPObject *npobj, NPString *script,
                                NPVariant *result)
{
    if (NPNFuncs.evaluate != NULL) {
        return NPNFuncs.evaluate(npp, npobj, script, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                                   NPVariant *result)
{
    if (NPNFuncs.getproperty != NULL) {
        return NPNFuncs.getproperty(npp, npobj, propertyName, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::SetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
                                   const NPVariant *value)
{
    if (NPNFuncs.setproperty != NULL) {
        return NPNFuncs.setproperty(npp, npobj, propertyName, value);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::RemoveProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
    if (NPNFuncs.removeproperty != NULL) {
        return NPNFuncs.removeproperty(npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
    if (NPNFuncs.hasproperty != NULL) {
        return NPNFuncs.hasproperty(npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasMethod(NPP npp, NPObject *npobj, NPIdentifier methodName)
{
    if (NPNFuncs.hasmethod != NULL) {
        return NPNFuncs.hasmethod(npp, npobj, methodName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Enumerate(NPP npp, NPObject *npobj, NPIdentifier **identifier,
                                 uint32_t *count)
{
    if (NPNFuncs.enumerate != NULL) {
        return NPNFuncs.enumerate(npp, npobj, identifier, count);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Construct(NPP npp, NPObject *npobj, const NPVariant *args,
                                 uint32_t argCount, NPVariant *result)
{
    if (NPNFuncs.construct != NULL) {
        return NPNFuncs.construct(npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

void NpapiBrowserHost::SetException(NPObject *npobj, const NPUTF8 *message)
{
    if (NPNFuncs.setexception != NULL) {
        NPNFuncs.setexception(npobj, message);
    }
}