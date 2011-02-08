/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FireBreathWin.cpp : Implementation of DLL Exports.

#include "win_common.h"
#include "global/resource.h"
#include "FireBreathWin_i.h"
#include "axmain.h"
#include "FBControl.h"
#include "axutil.h"

using FB::ActiveX::isStaticInitialized;
using FB::ActiveX::flagStaticInitialized;
using FB::ActiveX::FbPerUserRegistration;

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	HRESULT hr = _AtlModule.DllCanUnloadNow();
	if (hr == S_OK && isStaticInitialized()) {
	    getFactoryInstance()->globalPluginDeinitialize();
        FB::Log::stopLogging();
		flagStaticInitialized(false);
	}
    return hr;
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT hr = _AtlModule.DllGetClassObject(rclsid, riid, ppv);
	if (SUCCEEDED(hr) && !isStaticInitialized()) {
        FB::Log::initLogging();
	    getFactoryInstance()->globalPluginInitialize();
		flagStaticInitialized(true);
	}
	return hr;
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    //Sleep(10000);
    // registers object, typelib and all interfaces in typelib
#ifndef FB_ATLREG_MACHINEWIDE
    FbPerUserRegistration perUser(true);
#endif
    HRESULT hr = _AtlModule.DllRegisterServer();

    if (!SUCCEEDED(hr))
        hr = getFactoryInstance()->UpdateWindowsRegistry(true);
    return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
#ifndef FB_ATLREG_MACHINEWIDE
    FbPerUserRegistration perUser(true);
#endif
    HRESULT hr = _AtlModule.DllUnregisterServer();
    if (!SUCCEEDED(hr))
        hr = getFactoryInstance()->UpdateWindowsRegistry(false);
    return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.    
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");
    bool doPerUserRegistration = false;

    if (pszCmdLine != NULL)
    {
        if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
        {
            doPerUserRegistration = true;
        }
    }

    FbPerUserRegistration perUser(doPerUserRegistration);

    if (bInstall)
    {   
        hr = DllRegisterServer();
        if (FAILED(hr))
        {   
            DllUnregisterServer();
        }
    }
    else
    {
        hr = DllUnregisterServer();
    }

    return hr;
}

