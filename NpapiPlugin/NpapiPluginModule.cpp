/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <stdexcept>

#include "NpapiPluginModule.h"

using namespace FB::Npapi;

NpapiPluginModule::NpapiPluginModule(void)
{
    memset(&NPNFuncs, 0, sizeof(NPNetscapeFuncs));
}

NpapiPluginModule::~NpapiPluginModule(void)
{
}

void NpapiPluginModule::setNetscapeFuncs(NPNetscapeFuncs *npnFuncs)
{
    copyNPBrowserFuncs(&NPNFuncs, npnFuncs);
}

/* npapi.h definitions */
void NpapiPluginModule::Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor)
{
    *plugin_major = NP_VERSION_MAJOR;
    *plugin_minor = NP_VERSION_MINOR;
    *netscape_major = ( ((uint32_t)NPNFuncs.version) & 0xFF00 ) >> 8;
    *netscape_minor = (int)(((uint32_t)NPNFuncs.version) & 0xFF);
}


void* NpapiPluginModule::MemAlloc(uint32 size)
{
    if (NPNFuncs.memalloc != NULL) {
        return NPNFuncs.memalloc(size);
    } else {
        return NULL;
    }
}

void NpapiPluginModule::MemFree(void* ptr)
{
    if (NPNFuncs.memfree != NULL) {
        NPNFuncs.memfree(ptr);
    }
}

uint32 NpapiPluginModule::MemFlush(uint32 size)
{
    if (NPNFuncs.memflush != NULL) {
        return NPNFuncs.memflush(size);
    } else {
        return 0;
    }
}

void NpapiPluginModule::ReloadPlugins(NPBool reloadPages)
{
    if (NPNFuncs.reloadplugins != NULL) {
        NPNFuncs.reloadplugins(reloadPages);
    }
}

void NpapiPluginModule::ReleaseVariantValue(NPVariant *variant)
{
    if (NPNFuncs.releasevariantvalue != NULL) {
        NPNFuncs.releasevariantvalue(variant);
    }
}

NPIdentifier NpapiPluginModule::GetStringIdentifier(const NPUTF8 *name)
{
    if (NPNFuncs.getstringidentifier != NULL) {
        return NPNFuncs.getstringidentifier(name);
    } else {
        return NULL;
    }
}

void NpapiPluginModule::GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
    if (NPNFuncs.getstringidentifiers != NULL) {
        NPNFuncs.getstringidentifiers(names, nameCount, identifiers);
    }
}

NPIdentifier NpapiPluginModule::GetIntIdentifier(int32_t intid)
{
    if (NPNFuncs.getintidentifier != NULL) {
        return NPNFuncs.getintidentifier(intid);
    } else {
        return NULL;
    }
}

bool NpapiPluginModule::IdentifierIsString(NPIdentifier identifier)
{
    if (NPNFuncs.identifierisstring != NULL) {
        return NPNFuncs.identifierisstring(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPUTF8 *NpapiPluginModule::UTF8FromIdentifier(NPIdentifier identifier)
{
    if (NPNFuncs.utf8fromidentifier != NULL) {
        return NPNFuncs.utf8fromidentifier(identifier);
    } else {
        return NULL;
    }
}

int32_t NpapiPluginModule::IntFromIdentifier(NPIdentifier identifier)
{
    if (NPNFuncs.intfromidentifier != NULL) {
        return NPNFuncs.intfromidentifier(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPObject *NpapiPluginModule::RetainObject(NPObject *npobj)
{
    if (NPNFuncs.retainobject != NULL) {
        return NPNFuncs.retainobject(npobj);
    } else {
        return npobj;
    }
}

void NpapiPluginModule::ReleaseObject(NPObject *npobj)
{
    if (NPNFuncs.releaseobject != NULL) {
        NPNFuncs.releaseobject(npobj);
    }
}