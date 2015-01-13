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

#include "logging.h"
#include "D3d10AsyncDrawService.h"
#include "D3d10DeviceContext.h"
#include "BrowserHost.h"
#include "FactoryBase.h"

using namespace FB;


D3d10AsyncDrawService::D3d10AsyncDrawService(BrowserHostPtr host)
    : m_weakHost(host)
    , m_dc(getFactoryInstance()->createD3d10DeviceContext())
    , m_width(0)
    , m_height(0)
    , m_dimsChanged(false)
{
    assert(m_dc);   // the factory shouldn't fail
}

D3d10AsyncDrawService::~D3d10AsyncDrawService()
{
    if (ID3D10Device1* dev = device()) {
        dev->ClearState();
    }
}

void D3d10AsyncDrawService::resized(uint32_t width, uint32_t height)
{
    m_dimsChanged = (m_width != width) || (m_height != height);

    uint32_t was = m_width * m_height;
    uint32_t now = width * height;

    m_width = width; m_height = height;

    if (was == 0 && now != 0) {
        present(true);
    }
}

ID3D10Device1* D3d10AsyncDrawService::device() const
{
    return m_dc ? m_dc->device() : 0;
}

void D3d10AsyncDrawService::render(RenderCallback cb)
{
    ID3D10Device1* dev = device();
    if (!dev)
        return;

    boost::unique_lock<boost::mutex> lock(m_mut);
    while (!m_pBackBuffer)
    {
        m_cond.wait(lock);
    }

    CComPtr<ID3D10RenderTargetView> rtView;
    HRESULT hr = dev->CreateRenderTargetView(m_pBackBuffer, NULL, &rtView);
    if (SUCCEEDED(hr)) {
        D3D10_TEXTURE2D_DESC desc;
        m_pBackBuffer->GetDesc(&desc);

        cb(dev, rtView, desc.Width, desc.Height);

        BrowserHostPtr host(m_weakHost.lock());
        if (host) {
            if (host->isMainThread()) {
                present(false);
            } else {
                host->ScheduleOnMainThread(this->shared_from_this(), boost::bind(&D3d10AsyncDrawService::present, this, false));
            }
        }
    }
}


