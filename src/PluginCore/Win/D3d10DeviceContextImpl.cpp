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

#include "D3d10DeviceContextImpl.h"
#include "logging.h"
#include <iomanip>

using namespace FB;

typedef HRESULT (WINAPI *CreateDXGIFactory1Func)(
    REFIID riid,
    void **ppFactory
    );

///////////////

static void fblog_hr(const char* what, HRESULT hr)
{
    std::stringstream ss;
    ss << what << " failed: 0x" << std::setfill('0') << std::setw(8) << std::hex << hr;
    FBLOG_ERROR("getD3D10Device", ss);
}

D3d10DeviceContextImpl::D3d10DeviceContextImpl()
    :_d3d10(LoadLibraryA("d3d10_1.dll"))
    ,_dxgi(LoadLibraryA("dxgi.dll"))
{
    if (!_d3d10 || !_dxgi) {
        FBLOG_ERROR("D3d10DeviceContextImpl", "missing dlls");
        return;
    }

    D3D10CreateDevice1Func createD3DDevice = (D3D10CreateDevice1Func) GetProcAddress(_d3d10, "D3D10CreateDevice1");
    CreateDXGIFactory1Func createDXGIFactory1 = (CreateDXGIFactory1Func) GetProcAddress(_dxgi, "CreateDXGIFactory1");
    
    if (!createD3DDevice || !createDXGIFactory1) {
        FBLOG_ERROR("D3d10DeviceContextImpl", "missing procs");
        return;
    }

    // Try to use a DXGI 1.1 adapter in order to share resources across processes.
    CComPtr<IDXGIFactory1> factory1;
    HRESULT hr = createDXGIFactory1(__uuidof(IDXGIFactory1), (void**) &factory1);

    if (FAILED(hr) || !factory1) {
        fblog_hr("CreateDXGIFactory1", hr);
        return;
    }

    CComPtr<IDXGIAdapter1> adapter1;
    hr = factory1->EnumAdapters1(0, &adapter1);
    if (FAILED(hr) || !adapter1) {
        fblog_hr("EnumAdapters1", hr);
    }

    hr = createDevice(createD3DDevice, adapter1);
    if (FAILED(hr)) {
        fblog_hr("D3D10CreateDevice1", hr);
    }
}

HRESULT D3d10DeviceContextImpl::createDevice(D3D10CreateDevice1Func func, CComPtr<IDXGIAdapter1> adapter)
{
    return func(
        adapter, 
        D3D10_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D10_CREATE_DEVICE_BGRA_SUPPORT |
        D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS,
        D3D10_FEATURE_LEVEL_10_0,
        D3D10_1_SDK_VERSION,
        &_dev);
}

D3d10DeviceContextImpl::~D3d10DeviceContextImpl()
{
    _dev.Release();
    if (_dxgi) FreeLibrary(_dxgi);
    if (_d3d10) FreeLibrary(_d3d10);
}

CComPtr<ID3D10Device1> D3d10DeviceContextImpl::device()
{
    return _dev;
}

