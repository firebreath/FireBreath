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

void initPluginModule()
{
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
    NpapiPluginModule module;
    return module.NPP_GetValue((NPP_t *)future, variable, value);
}

extern "C" NPError NP_Initialize(NPNetscapeFuncs* pFuncs
                             , NPPluginFuncs *pluginFuncs)
{
    /* can't use FBLOG_XXX before GetModule returns, as it calls InitLogging */
    initPluginModule();
    NpapiPluginModule *module = NpapiPluginModule::GetModule(0);
    module->getPluginFuncs(pluginFuncs);
    module->setNetscapeFuncs(pFuncs);

    FBLOG_INFO("NPAPI", "Initialization done");
    return NPERR_NO_ERROR;
}

extern "C" NPError NP_Shutdown()
{
    FBLOG_INFO("NPAPI", "");
    NpapiPluginModule::ReleaseModule(0);
    return NPERR_NO_ERROR;
}

