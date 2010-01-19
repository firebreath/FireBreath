/**********************************************************\ 
  Default auto-generated np_winmain.cpp

  This file contains the entrypoints for a windows NPAPI
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
	void NP_Shutdown(void);
    
#ifndef _NO_MAIN
	// For compatibility with CFM browsers.
	int main(NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown);
#endif
}

#pragma GCC visibility pop

#ifndef _NO_MAIN
int main(NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown)
{
    printf("Main called");
    NP_Initialize(browserFuncs);
    NP_GetEntryPoints(pluginFuncs);
    *shutdown = (NPP_ShutdownProcPtr)&NP_Shutdown;
}
#endif

void initPluginModule()
{
    if (module == NULL) {
        module = new NpapiPluginModule();
        NpapiPluginModule::Default = module;
    }
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    printf("GetEntryPoints called");
    initPluginModule();
    module->getPluginFuncs(pFuncs);
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    printf("Initialize called");
    initPluginModule();
    module->setNetscapeFuncs(pFuncs);

    return NPERR_NO_ERROR;
}

void OSCALL NP_Shutdown()
{
    delete module;
}
