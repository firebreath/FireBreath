/**********************************************************\
  Default auto-generated np_x11main.cpp

  This file contains the entrypoints for a *nix NPAPI
  plugin.  Don't change this unless you know what you're
  doing!
\**********************************************************/

#include "global/config.h"
#include "NpapiTypes.h"
#include "NpapiPluginModule.h"

using namespace FB::Npapi;
FB::Npapi::NpapiPluginModule *module = NULL;

void initPluginModule()
{
    if (module == NULL) {
        module = new NpapiPluginModule();
        NpapiPluginModule::Default = module;
    }
}

extern "C" char * NP_GetPluginVersion()
{
    return (char *)FBSTRING_PLUGIN_VERSION;
}

extern "C" const char * NP_GetMIMEDescription()
{
    return (char *)FBSTRING_X11MIMEType;
}

extern "C" NPError NP_GetValue(void *future, NPPVariable variable, void *value)
{
    // Create a temporary npapipluginmodule -- false means don't initialize everything
    NpapiPluginModule module(false);
    return module.NPP_GetValue((NPP_t *)future, variable, value);
}

extern "C" NPError NP_Initialize(NPNetscapeFuncs* pFuncs
                             , NPPluginFuncs *pluginFuncs)
{
    initPluginModule();
    module->getPluginFuncs(pluginFuncs);
    module->setNetscapeFuncs(pFuncs);

    return NPERR_NO_ERROR;
}

extern "C" NPError NP_Shutdown()
{
    delete module;
    module = NULL;
    return NPERR_NO_ERROR;
}

