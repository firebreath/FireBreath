/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "windows.h"

#include "npupp.h"
#include "NpapiPluginModule.h"

using namespace FB::Npapi;
FB::Npapi::NpapiPluginModule *module = NULL;

void initPluginModule()
{
    if (module == NULL)
        module = new NpapiPluginModule();
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
    return NPERR_NO_ERROR;
}
