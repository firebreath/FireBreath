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
HINSTANCE gInstance(NULL);
std::string g_dllPath;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (!gInstance) {
        USES_CONVERSION;
        gInstance = hInstance;
        LPTSTR strDLLPath1 = new TCHAR[_MAX_PATH];
        ::GetModuleFileName(gInstance, strDLLPath1, _MAX_PATH);
         
        g_dllPath = CW2A(strDLLPath1);
    }
    return _AtlModule.DllMain(dwReason, lpReserved);
}
