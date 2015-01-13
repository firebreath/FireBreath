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

#include <boost/bind.hpp>
#include "BrowserHost.h"
#include "ActiveXAsyncDrawService.h"
#include "logging.h"

using namespace FB::ActiveX;

ActiveXAsyncDrawService::ActiveXAsyncDrawService(FB::BrowserHostPtr host, IViewObjectPresentSite* vops)
    : D3d10AsyncDrawService(host)
    , m_pViewObjectPresentSite(vops)
{
    assert(vops);
}

void ActiveXAsyncDrawService::present(bool initOnly)
{
    ID3D10Device1* dev = device();
    if (!dev)
        return;

    if (m_pSurfacePresenter) {
        BOOL isCurrent = 0;
        m_pSurfacePresenter->IsCurrent(&isCurrent);
        if (!isCurrent || m_dimsChanged) {
            // don't bother presenting, because the new 
            // SurfacePresenter will replace it with blank
            m_pSurfacePresenter = 0;
        } else {
            if (!initOnly) {
                m_pSurfacePresenter->Present(0, NULL);
            }
        }
    } 
    
    if (!m_pSurfacePresenter) {
        // (re)create surface presenter
        m_dimsChanged = false;
        UINT backBuffers = 1;
        DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;    // IE9 only supports this one
        HRESULT hr = m_pViewObjectPresentSite->CreateSurfacePresenter(
            dev, m_width, m_height, backBuffers, format, VIEW_OBJECT_ALPHA_MODE_PREMULTIPLIED, &m_pSurfacePresenter);
        if (FAILED(hr) || !m_pSurfacePresenter) {
            FBLOG_ERROR("ActiveXAsyncDrawService::present", "CreateSurfacePresenter failed");
            return;
        }
    } 

    // send the back buffer to the render thread
    {
        boost::lock_guard<boost::mutex> lock(m_mut);
        REFIID riid = __uuidof(ID3D10Texture2D);
        m_pSurfacePresenter->GetBuffer(0, riid, (void**) &m_pBackBuffer.p);
    }
    m_cond.notify_one();
}

