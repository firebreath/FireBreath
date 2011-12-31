#include "win_common.h"

#include "NpapiTypes.h"
#include "NpapiPluginModule.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB::Npapi;

void initPluginModule()
{
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    FBLOG_INFO("NPAPI", "");
    initPluginModule();
    NpapiPluginModule *module = NpapiPluginModule::GetModule(0);
    module->getPluginFuncs(pFuncs);
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    FBLOG_INFO("NPAPI", "");
    initPluginModule();
    NpapiPluginModule *module = NpapiPluginModule::GetModule(0);
    module->setNetscapeFuncs(pFuncs);

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
    FBLOG_INFO("NPAPI", "");
    NpapiPluginModule::ReleaseModule(0);
    return NPERR_NO_ERROR;
}

