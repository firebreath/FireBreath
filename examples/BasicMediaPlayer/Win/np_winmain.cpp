/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    New BSD License
            http://www.opensource.org/licenses/bsd-license.php

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "Win/win_common.h"

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
    return NPERR_NO_ERROR;
}
