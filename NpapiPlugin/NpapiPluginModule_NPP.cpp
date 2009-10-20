/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NpapiPluginModule.h"
#include "NpapiPlugin.h"

using namespace FB::Npapi;

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
NPError NpapiPluginModule::NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc,
                                   char* argn[], char* argv[], NPSavedData* saved)
{
    if (instance == NULL) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = new NpapiPlugin();
    if (plugin == NULL)
        return NPERR_OUT_OF_MEMORY_ERROR;

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
                                       NPBool seekable, uint16* stype)
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

int32 NpapiPluginModule::NPP_WriteReady(NPP instance, NPStream* stream)
{
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NpapiPlugin *plugin = getPlugin(instance);
    return plugin->WriteReady(stream);
}

int32 NpapiPluginModule::NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len,
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

int16 NpapiPluginModule::NPP_HandleEvent(NPP instance, void* event)
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