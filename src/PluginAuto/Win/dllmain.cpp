// dllmain.cpp : Implementation of DllMain.

#include <boost/scoped_array.hpp>
#include "win_common.h"
#include "axmain.h"
#include "axutil.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

HINSTANCE gInstance(NULL);
std::string g_dllPath;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (!gInstance) {
        gInstance = hInstance;
        boost::scoped_array<TCHAR> strDLLPath1(new TCHAR[_MAX_PATH]);
        ::GetModuleFileName(gInstance, strDLLPath1.get(), _MAX_PATH);
        
        g_dllPath = FB::wstring_to_utf8(strDLLPath1.get());
    }
    return _AtlModule.DllMain(dwReason, lpReserved);
}

