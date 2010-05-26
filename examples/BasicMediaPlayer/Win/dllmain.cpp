/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    New BSD License
            http://www.opensource.org/licenses/bsd-license.php

Copyright 2009 Richard Bateman, Firebreath development team
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
