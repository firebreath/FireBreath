/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <cstdio>
#include "config.h"
#include "NpapiPluginModule.h"
#include "NpapiPlugin.h"
#include "FactoryDefinitions.h"

using namespace FB::Npapi;

NpapiPluginModule *NpapiPluginModule::Default = NULL;

inline NpapiPlugin *getPlugin(NPP instance)
{
    return static_cast<NpapiPlugin *>(instance->pdata);
}

inline bool validInstance(NPP instance)
{
    return instance != NULL && instance->pdata != NULL;
}

// These are the static NPP_ functions; NPP_New and NPP_Destroy create and destroy the
// plugin, the rest are wrappers that dereference NPP->pdata to get at the plugin object
// and proxy the call to there.
NPError NpapiPluginModule::NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
                                   char* argn[], char* argv[], NPSavedData* saved)
{
    if (instance == NULL) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    FB::AutoPtr<NpapiBrowserHost> host = new NpapiBrowserHost(NpapiPluginModule::Default, instance);
    host->setBrowserFuncs(&(NpapiPluginModule::Default->NPNFuncs));
    NpapiPlugin *plugin(NULL);
    try {
        plugin = _getNpapiPlugin(host);
        plugin->init(pluginType, argc, argn, argv);
        if (plugin == NULL)
            return NPERR_OUT_OF_MEMORY_ERROR;
    } catch (PluginCreateError &e) {
        printf("%s\n", e.what());
        delete plugin;
        plugin = NULL;
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    instance->pdata = static_cast<void *>(plugin);
    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_Destroy(NPP instance, NPSavedData** save)
{
    if (instance == NULL) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);

    plugin->shutdown();
    delete plugin;

    instance->pdata = NULL;
    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_SetWindow(NPP instance, NPWindow* window)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);

    return plugin->SetWindow(window);
}

NPError NpapiPluginModule::NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
                                       NPBool seekable, uint16_t* stype)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->NewStream(type, stream, seekable, stype);
}

NPError NpapiPluginModule::NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->DestroyStream(stream, reason);
}

int32_t NpapiPluginModule::NPP_WriteReady(NPP instance, NPStream* stream)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->WriteReady(stream);
}

int32_t NpapiPluginModule::NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len,
                                 void* buffer)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->Write(stream, offset, len, buffer);
}

void NpapiPluginModule::NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    if (!validInstance(instance)) {
        return;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    plugin->StreamAsFile(stream, fname);
}

void NpapiPluginModule::NPP_Print(NPP instance, NPPrint* platformPrint)
{
    if (!validInstance(instance)) {
        return;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    plugin->Print(platformPrint);
}

int16_t NpapiPluginModule::NPP_HandleEvent(NPP instance, void* event)
{
    if (!validInstance(instance)) {
        return 0;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->HandleEvent(event);
}

void NpapiPluginModule::NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                                    void* notifyData)
{
    if (!validInstance(instance)) {
        return;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    plugin->URLNotify(url, reason, notifyData);
}

NPError NpapiPluginModule::NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    /* Some values are not dependent on having a valid plugin instance */
    switch(variable) {
        case NPPVpluginNameString:
            *((const char **)value) = FBSTRING_PluginName;
            return NPERR_NO_ERROR;
            break;
        case NPPVpluginDescriptionString:
            *((const char **)value) = FBSTRING_FileDescription;
            return NPERR_NO_ERROR;
            break;
        default:
            break;
    }
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->GetValue(variable, value);
}

NPError NpapiPluginModule::NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->SetValue(variable, value);
}
