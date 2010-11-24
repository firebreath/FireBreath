/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    New BSD License
            http://www.opensource.org/licenses/bsd-license.php

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// dllmain.cpp : Implementation of DllMain.


#include <boost/scoped_array.hpp>
#include "Win/win_common.h"
#include "resource.h"
#include "axmain.h"
#include "axutil.h"

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
