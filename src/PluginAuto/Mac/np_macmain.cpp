/**********************************************************\
  Default auto-generated np_winmain.cpp

  This file contains the entrypoints for a Mac NPAPI
  plugin.  Don't change this unless you know what you're
  doing!
\**********************************************************/

#include "NpapiTypes.h"
#include "NpapiPluginModule.h"

using namespace FB::Npapi;
FB::Npapi::NpapiPluginModule *module = NULL;

typedef void (*NPP_ShutdownProcPtr)(void);

#pragma GCC visibility push(default)

extern "C" {
    NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
    NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
    NPError NP_Shutdown(void);
}

#pragma GCC visibility pop

void initPluginModule()
{
    if (module == NULL) {
        module = new NpapiPluginModule();
        NpapiPluginModule::Default = module;
    }
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    FBLOG_TRACE("PluginCore", __func__);
    initPluginModule();
    module->getPluginFuncs(pFuncs);
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    FBLOG_TRACE("PluginCore", __func__);
    initPluginModule();
    module->setNetscapeFuncs(pFuncs);

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
    FBLOG_TRACE("PluginCore", __func__);
    delete module;
    module = NULL;
    return NPERR_NO_ERROR;
}
