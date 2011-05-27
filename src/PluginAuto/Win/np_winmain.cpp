#include "win_common.h"

#include "NpapiTypes.h"
#include "NpapiPluginModule.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB::Npapi;
FB::Npapi::NpapiPluginModule * module = NULL;

void initPluginModule()
{
    if (module == NULL) {
        module = new NpapiPluginModule();
        NpapiPluginModule::Default = module;
    }
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    initPluginModule();
    module->getPluginFuncs(pFuncs);
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    initPluginModule();
    module->setNetscapeFuncs(pFuncs);

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
    delete module;
    module = NULL;
    return NPERR_NO_ERROR;
}

