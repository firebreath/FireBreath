/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifdef FB_MACOSX
#include <dlfcn.h>
#endif
#include <cstdio>
#include "NpapiPlugin.h"
#include "FactoryBase.h"
#include "NpapiBrowserHost.h"
#include <boost/shared_ptr.hpp>
#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "AsyncFunctionCall.h"
#include "PluginInfo.h"
#include "SafeQueue.h"
#include "NpapiPluginModule.h"

#if FB_WIN
#  include "Win/NpapiBrowserHostAsyncWin.h"
#elif FB_MACOSX
#  include "Mac/OneShotManager.h"
#endif

using namespace FB::Npapi;

namespace
{
    bool needAsyncCallsWorkaround(NPP npp, NPNetscapeFuncs* funcs)
    {
        bool result(false);
        // work-around detection here
#if FB_WIN
        const char* const cstrUserAgent = funcs->uagent(npp);
        if(cstrUserAgent) {
            const std::string userAgent(cstrUserAgent);
            // Safari 5.1 NPN_PluginThreadAsyncCall doesn't seem to work anymore; use the workaround
            result = userAgent.find("Safari") != std::string::npos;
        }
#endif
        return result || (funcs->version < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL);
    }

    bool asyncCallsWorkaround(NPP npp, NPNetscapeFuncs* funcs = 0)
    {
        static const bool useWorkaround = (funcs) ? needAsyncCallsWorkaround(npp, funcs) : false;
        //static const bool useWorkaround = false;
        return useWorkaround;
    }

    NpapiBrowserHostPtr createBrowserHost(NpapiPluginModule* module, NPP npp)
    {
        try {
            NpapiBrowserHostPtr host;
            NPNetscapeFuncs& npnFuncs = module->NPNFuncs;

            if(asyncCallsWorkaround(npp, &npnFuncs)) {
                npnFuncs.pluginthreadasynccall = NULL;
    #if FB_WIN
                NpapiBrowserHostPtr host(boost::make_shared<NpapiBrowserHostAsyncWin>(module, npp));
                return host;
    #else
                // no work-around for this platform
                NpapiBrowserHostPtr host(boost::make_shared<NpapiBrowserHost>(module, npp));
                return host;
    #endif
            } else {
                NpapiBrowserHostPtr host(boost::make_shared<NpapiBrowserHost>(module, npp));
                return host;
            }
        } catch (...) {
            // This function must not return an exception
            return NpapiBrowserHostPtr();
        }
    }

    class NpapiPDataHolder
    {
    private:
        NpapiBrowserHostPtr m_host;
        boost::shared_ptr<NpapiPlugin> m_plugin;

    public:
        NpapiPDataHolder(NpapiBrowserHostPtr host, boost::shared_ptr<NpapiPlugin> plugin)
          : m_host(host), m_plugin(plugin)
        {
#ifdef FB_MACOSX
            FB::OneShotManager::getInstance().npp_register(m_host->getContextID());
#endif
        }
        ~NpapiPDataHolder() {
#ifdef FB_MACOSX
            FB::OneShotManager::getInstance().npp_unregister(m_host->getContextID());
#endif
        }

        NpapiBrowserHostPtr getHost() const {
            return m_host;
        }
        NpapiPluginPtr getPlugin() const {
            return m_plugin;
        }
    };

    bool validInstance(NPP instance)
    {
        return instance != NULL && instance->pdata != NULL;
    }

    NpapiPDataHolder* getHolder(NPP instance)
    {
        if (validInstance(instance))
            return static_cast<NpapiPDataHolder*>(instance->pdata);
        return NULL;
    }
}

NpapiPluginPtr FB::Npapi::getPlugin(NPP instance)
{
    if (NpapiPDataHolder* holder = getHolder(instance))
        return holder->getPlugin();
    return NpapiPluginPtr();
}

#ifdef FB_MACOSX
// This is used on mac snow leopard safari
void NpapiPluginModule::scheduleAsyncCallback(NPP instance, void (*func)(void *), void *userData)
{
    FB::OneShotManager::getInstance().npp_scheduleAsyncCallback(instance, func, userData);
}
#endif

// These are the static NPP_ functions; NPP_New and NPP_Destroy create and destroy the
// plugin, the rest are wrappers that dereference NPP->pdata to get at the plugin object
// and proxy the call to there.
NPError NpapiPluginModule::NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
                                   char* argn[], char* argv[], NPSavedData* saved)
{
    FBLOG_INFO("NPAPI", "NPP_New: " << (void*) instance);
    if (instance == NULL) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    try
    {
#ifdef FB_MACOSX
        // Helps with certain weird embedding cases
        Dl_info info;

        dladdr(__builtin_return_address(0), &info);

        NpapiPluginModule *module = NpapiPluginModule::GetModule(info.dli_fbase);
#else
        NpapiPluginModule *module = NpapiPluginModule::GetModule(0);
#endif
        //printf("%p NpapiPluginModule::%s()\n", module, __func__);
        NPNetscapeFuncs& npnFuncs = module->NPNFuncs;

        NpapiBrowserHostPtr host(createBrowserHost(module, instance));

        host->setBrowserFuncs(&(npnFuncs));

        // TODO: We should probably change this and pass the MIMEType into _getNpapiPlugin instead
        // of into init later so that we can optionally return a different plugin type depending
        // on the specific mimetype
        NpapiPluginPtr plugin(getFactoryInstance()->createNpapiPlugin(host, pluginType));
        if (!plugin) {
            return NPERR_OUT_OF_MEMORY_ERROR;
        }

        NpapiPDataHolder* holder = new NpapiPDataHolder(host, plugin);
        instance->pdata = static_cast<void*>(holder);

        plugin->init(pluginType, argc, argn, argv);
    }
    catch (const PluginCreateError &e)
    {
        printf("%s\n", e.what());
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    catch (const std::bad_alloc& e)
    {
        printf("%s\n", e.what());
        return NPERR_OUT_OF_MEMORY_ERROR;
    }
    catch (const std::exception& e)
    {
        printf("%s\n", e.what());
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_Destroy(NPP instance, NPSavedData** save)
{
    FBLOG_INFO("NPAPI", "NPP_Destroy: " << (void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    NpapiBrowserHostWeakPtr weakHost;

    if (NpapiPDataHolder* holder = getHolder(instance)) {
        NpapiBrowserHostPtr host(holder->getHost());
        weakHost = host;
        if (host)
            host->shutdown();

        if (NpapiPluginPtr plugin = holder->getPlugin())
            plugin->shutdown();

        instance->pdata = NULL;
        delete holder; // Destroy plugin
        // host should be destroyed when it goes out of scope here
    } else {
        return NPERR_GENERIC_ERROR;
    }
    // If this assertion fails, you probably have a circular reference
    // to your BrowserHost object somewhere -- the host should be gone
    // by this point. This assertion is warning you of a bug.
    assert(weakHost.expired());

    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_SetWindow(NPP instance, NPWindow* window)
{
    FBLOG_TRACE("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (pluginGuiEnabled())
        if (NpapiPluginPtr plugin = getPlugin(instance)) {
            return plugin->SetWindow(window);
        }

    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
                                       NPBool seekable, uint16_t* stype)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->NewStream(type, stream, seekable, stype);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiPluginModule::NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->DestroyStream(stream, reason);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32_t NpapiPluginModule::NPP_WriteReady(NPP instance, NPStream* stream)
{
    FBLOG_INFO("NPAPI",(void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->WriteReady(stream);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32_t NpapiPluginModule::NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len,
                                 void* buffer)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->Write(stream, offset, len, buffer);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void NpapiPluginModule::NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        plugin->StreamAsFile(stream, fname);
    }
}

void NpapiPluginModule::NPP_Print(NPP instance, NPPrint* platformPrint)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        plugin->Print(platformPrint);
    }
}

int16_t NpapiPluginModule::NPP_HandleEvent(NPP instance, void* event)
{
    FBLOG_TRACE("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return 0;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->HandleEvent(event);
    } else {
        return 0;
    }
}

void NpapiPluginModule::NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                                    void* notifyData)
{
    FBLOG_INFO("NPAPI", (void*) instance);
    if (!validInstance(instance)) {
        return;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        plugin->URLNotify(url, reason, notifyData);
    }
}

NPError NpapiPluginModule::NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    FBLOG_TRACE("NPAPI", (void*) instance);
    // These values may depend on the mimetype of the plugin
    if (!validInstance(instance)) {
        switch (variable) {
        case NPPVpluginNameString: {
            static const std::string pluginName = getFactoryInstance()->getPluginName("");
            *((const char **)value) = pluginName.c_str();
            break; }
        case NPPVpluginDescriptionString: {
            // If nothing is instantiated, use the first description found
            static const std::string pluginDesc = getFactoryInstance()->getPluginDescription("");
            *((const char **)value) = pluginDesc.c_str();
            break; }
        default:
            return NPERR_GENERIC_ERROR;
        }
        return NPERR_NO_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->GetValue(variable, value);
    }

    return NPERR_NO_ERROR;
}

NPError NpapiPluginModule::NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    FBLOG_TRACE("NPAPI", (void*) instance << "variable: " << (void*)variable);
    if (!validInstance(instance)) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (NpapiPluginPtr plugin = getPlugin(instance)) {
        return plugin->SetValue(variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

