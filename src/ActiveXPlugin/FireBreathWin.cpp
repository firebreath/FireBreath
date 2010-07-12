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

#include "Win/win_common.h"
#include "resource.h"
#include "FireBreathWin_i.h"
#include "axmain.h"
#include "dlldatax.h"
#include "FBControl.h"
#include "axutil.h"

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    //Sleep(10000);
    // registers object, typelib and all interfaces in typelib
    FbPerUserRegistration perUser(true);
    HRESULT hr = _AtlModule.DllRegisterServer();

    if (!SUCCEEDED(hr))
        hr = CFBControl::UpdateRegistry(true);
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
    return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
    FbPerUserRegistration perUser(true);
    HRESULT hr = _AtlModule.DllUnregisterServer();
    if (!SUCCEEDED(hr))
        hr = CFBControl::UpdateRegistry(false);
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
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


