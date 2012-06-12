/**********************************************************\
  Default auto-generated np_winmain.cpp

  This file contains the entrypoints for a Mac NPAPI
  plugin.  Don't change this unless you know what you're
  doing!
\**********************************************************/

#include "NpapiTypes.h"
#include "NpapiPluginModule.h"

#include <dlfcn.h>
#include <sys/sysctl.h>

#ifndef NDEBUG
#define WAIT_FOR_DEBUGGER 0
#else
#define WAIT_FOR_DEBUGGER 0
#endif

#if WAIT_FOR_DEBUGGER
static bool beingDebugged() {
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()}; size_t mib_size = 4;
    struct kinfo_proc kp; size_t kp_size = sizeof(kp);
    int result = sysctl(mib, mib_size, &kp, &kp_size, NULL, 0);
    return (0 == result) ? (P_TRACED & kp.kp_proc.p_flag) : false;
}
#endif

using namespace FB::Npapi;

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
#if WAIT_FOR_DEBUGGER
    #warning "WILL BLOCK ON P_TRACED"
    while (!beingDebugged())
        sleep(1);
#endif
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    FBLOG_INFO("NPAPI", "");
    initPluginModule();
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    NpapiPluginModule *module = NpapiPluginModule::GetModule(info.dli_fbase);
    module->getPluginFuncs(pFuncs);
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    /* can't use FBLOG_XXX before GetModule returns, as it calls InitLogging */
    initPluginModule();
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    NpapiPluginModule *module = NpapiPluginModule::GetModule(info.dli_fbase);
    module->setNetscapeFuncs(pFuncs);

    FBLOG_INFO("NPAPI", "Initialization done");
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
    FBLOG_INFO("NPAPI", "");
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    NpapiPluginModule::ReleaseModule(info.dli_fbase);
    return NPERR_NO_ERROR;
}
