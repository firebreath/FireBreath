/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// dllmain.cpp : Implementation of DllMain.


#include "win_common.h"
#include "resource.h"
#include "axmain.h"
#include "axutil.h"

STDAPI DllRegisterServer(void);

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	FbPerUserRegistration perUser(true);
    
    return _AtlModule.DllMain(dwReason, lpReserved); 
}
