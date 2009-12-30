/**********************************************************\ 
  Default auto-generated dllmain.cpp

  This file contains DLLMain; don't change this unless you
  know what you're doing!
\**********************************************************/

// dllmain.cpp : Implementation of DllMain.


#include "Win/win_common.h"
#include "resource.h"
#include "axmain.h"
#include "axutil.h"

STDAPI DllRegisterServer(void);

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    return _AtlModule.DllMain(dwReason, lpReserved); 
}
