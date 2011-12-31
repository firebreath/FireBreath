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

#include <stdexcept>

#include <cassert>
#include <boost/thread.hpp>
#include "logging.h"
#include "BrowserHost.h"
#include "PluginCore.h"
#include <assert.h>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "NpapiPluginModule.h"
using namespace FB::Npapi;

volatile uint32_t NpapiPluginModule::PluginModuleInitialized(0);
NpapiPluginModule::Modules NpapiPluginModule::m_modules;

NpapiPluginModule* NpapiPluginModule::GetModule(const void* key) {
    if (!NpapiPluginModule::PluginModuleInitialized) {
        FB::Log::initLogging();
        getFactoryInstance()->globalPluginInitialize();
    }
    NpapiPluginModule* rval = NULL;
    Modules::iterator module = m_modules.find(key);
    if (m_modules.end() == module) {
        rval = new NpapiPluginModule();
        m_modules[key] = rval;
        NpapiPluginModule::PluginModuleInitialized++;
    } else
        rval = module->second;
    return rval;
}
void NpapiPluginModule::ReleaseModule(const void* key) {
    Modules::iterator module = m_modules.find(key);
    if (m_modules.end() != module) {
        delete module->second;
        m_modules.erase(module);
        NpapiPluginModule::PluginModuleInitialized--;
    }

    if (!NpapiPluginModule::PluginModuleInitialized) {

        getFactoryInstance()->globalPluginDeinitialize();

        // NOTE: If this assertion fails you have some sort of memory leak; BrowserHost objects
        // are reference counted, so you have a shared_ptr to your browserhost sometime. This
        // can be a big problem because BrowserHost manages a lot of your memory and if you get
        // a new one for each instances on each page (including page reloads).
        assert(BrowserHost::getInstanceCount() == 0);
        assert(PluginCore::getActivePluginCount() == 0);
        FB::Log::stopLogging();
    }
}

NpapiPluginModule::NpapiPluginModule()
    : m_threadId(boost::this_thread::get_id())
{
    memset(&NPNFuncs, 0, sizeof(NPNetscapeFuncs));
}

NpapiPluginModule::~NpapiPluginModule()
{
}


void NpapiPluginModule::assertMainThread()
{
#ifdef _DEBUG
    assert(m_threadId == boost::this_thread::get_id());
#endif
}

void NpapiPluginModule::setNetscapeFuncs(NPNetscapeFuncs *npnFuncs)
{
    copyNPBrowserFuncs(&NPNFuncs, npnFuncs);
}

void NpapiPluginModule::getPluginFuncs(NPPluginFuncs* pFuncs)
{
    pFuncs->newp = &NpapiPluginModule::NPP_New;
    pFuncs->destroy = &NpapiPluginModule::NPP_Destroy;
    pFuncs->setwindow = &NpapiPluginModule::NPP_SetWindow;
    pFuncs->newstream = &NpapiPluginModule::NPP_NewStream;
    pFuncs->destroystream = &NpapiPluginModule::NPP_DestroyStream;
    pFuncs->asfile = &NpapiPluginModule::NPP_StreamAsFile;
    pFuncs->writeready = &NpapiPluginModule::NPP_WriteReady;
    pFuncs->write = &NpapiPluginModule::NPP_Write;
    pFuncs->print = &NpapiPluginModule::NPP_Print;
    pFuncs->event = &NpapiPluginModule::NPP_HandleEvent;
    pFuncs->urlnotify = &NpapiPluginModule::NPP_URLNotify;
    pFuncs->getvalue = &NpapiPluginModule::NPP_GetValue;
    pFuncs->setvalue = &NpapiPluginModule::NPP_SetValue;
}

/* npapi.h definitions */
void NpapiPluginModule::Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor)
{
    *plugin_major = NP_VERSION_MAJOR;
    *plugin_minor = NP_VERSION_MINOR;
    *netscape_major = ( ((uint32_t)NPNFuncs.version) & 0xFF00 ) >> 8;
    *netscape_minor = (int)(((uint32_t)NPNFuncs.version) & 0xFF);
}


void* NpapiPluginModule::MemAlloc(uint32_t size)
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

uint32_t NpapiPluginModule::MemFlush(uint32_t size)
{
    assertMainThread();
    if (NPNFuncs.memflush != NULL) {
        return NPNFuncs.memflush(size);
    } else {
        return 0;
    }
}

void NpapiPluginModule::ReloadPlugins(NPBool reloadPages)
{
    assertMainThread();
    if (NPNFuncs.reloadplugins != NULL) {
        NPNFuncs.reloadplugins(reloadPages);
    }
}

void NpapiPluginModule::ReleaseVariantValue(NPVariant *variant)
{
    assertMainThread();
    if (NPNFuncs.releasevariantvalue != NULL) {
        NPNFuncs.releasevariantvalue(variant);
    }
}

NPIdentifier NpapiPluginModule::GetStringIdentifier(const NPUTF8 *name)
{
    assertMainThread();
    if (NPNFuncs.getstringidentifier != NULL) {
        return NPNFuncs.getstringidentifier(name);
    } else {
        return NULL;
    }
}

void NpapiPluginModule::GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
    assertMainThread();
    if (NPNFuncs.getstringidentifiers != NULL) {
        NPNFuncs.getstringidentifiers(names, nameCount, identifiers);
    }
}

NPIdentifier NpapiPluginModule::GetIntIdentifier(int32_t intid)
{
    assertMainThread();
    if (NPNFuncs.getintidentifier != NULL) {
        return NPNFuncs.getintidentifier(intid);
    } else {
        return NULL;
    }
}

bool NpapiPluginModule::IdentifierIsString(NPIdentifier identifier)
{
    assertMainThread();
    if (NPNFuncs.identifierisstring != NULL) {
        return NPNFuncs.identifierisstring(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPUTF8 *NpapiPluginModule::UTF8FromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    if (NPNFuncs.utf8fromidentifier != NULL) {
        return NPNFuncs.utf8fromidentifier(identifier);
    } else {
        return NULL;
    }
}

// This is the preferred method to get strings from NPIdentifiers, since you
// don't have to worry about cleaning it up =]
std::string NpapiPluginModule::StringFromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    NPUTF8* idStr = UTF8FromIdentifier(identifier);
    std::string str;
    if (idStr != NULL)
        str = idStr;
    MemFree(idStr);
    return str;
}

int32_t NpapiPluginModule::IntFromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    if (NPNFuncs.intfromidentifier != NULL) {
        return NPNFuncs.intfromidentifier(identifier);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPObject *NpapiPluginModule::RetainObject(NPObject *npobj)
{
    assertMainThread();
    if (NPNFuncs.retainobject != NULL) {
        return NPNFuncs.retainobject(npobj);
    } else {
        return npobj;
    }
}

void NpapiPluginModule::ReleaseObject(NPObject *npobj)
{
    assertMainThread();
    if (NPNFuncs.releaseobject != NULL) {
        NPNFuncs.releaseobject(npobj);
    }
}

