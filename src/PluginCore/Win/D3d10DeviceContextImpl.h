/**********************************************************\ 
Original Author: Gil Gonen

Created:    Jan 28, 2013
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 Gil Gonen and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_D3D10DEVICECONTEXTIMPL
#define H_FB_D3D10DEVICECONTEXTIMPL

#include "D3d10DeviceContext.h"

namespace FB
{
    class D3d10DeviceContextImpl : public D3d10DeviceContext
    {
    public:
        D3d10DeviceContextImpl();
        virtual ~D3d10DeviceContextImpl();
        virtual CComPtr<ID3D10Device1> device();
    
        // the actual call to D3D10CreateDevice1 can be overridden:
        typedef HRESULT (WINAPI *D3D10CreateDevice1Func)(
            IDXGIAdapter *pAdapter,
            D3D10_DRIVER_TYPE DriverType,
            HMODULE Software,
            UINT Flags,
            D3D10_FEATURE_LEVEL1 HardwareLevel,
            UINT SDKVersion,
            ID3D10Device1 **ppDevice
            );
        virtual HRESULT createDevice(D3D10CreateDevice1Func func, CComPtr<IDXGIAdapter1> adapter);

    protected:
        CComPtr<ID3D10Device1> _dev;

    private:
        HMODULE _d3d10;
        HMODULE _dxgi;
    };
}

#endif
