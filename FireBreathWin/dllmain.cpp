// dllmain.cpp : Implementation of DllMain.

#include "win_common.h"
#include "resource.h"
#include "FireBreathWin_i.h"
#include "dllmain.h"
#include "dlldatax.h"

CFireBreathWinModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}
