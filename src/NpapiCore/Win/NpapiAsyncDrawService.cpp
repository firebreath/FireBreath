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

#include "NpapiAsyncDrawService.h"

using namespace FB::Npapi;


NpapiAsyncDrawService::NpapiAsyncDrawService(NpapiBrowserHostPtr host)
    : D3d10AsyncDrawService(host)
    , m_weakHost(host)
    , m_current(0)
{
}

// static
void NpapiAsyncDrawService::finalizeSurfaces(NpapiBrowserHostWeakPtr weakHost, Surface a, Surface b)
{
    NpapiBrowserHostPtr host = weakHost.lock();
    if (host) {
        a.finalize(host.get());
        b.finalize(host.get());
    }
}

NpapiAsyncDrawService::~NpapiAsyncDrawService()
{
    NpapiBrowserHostPtr host = m_weakHost.lock();
    if (host) {
        if (host->isMainThread()) {
            m_surface[0].finalize(host.get());
            m_surface[1].finalize(host.get());
        } else {
            boost::shared_ptr<void> dummy;
            host->ScheduleOnMainThread(dummy, boost::bind(finalizeSurfaces, m_weakHost, m_surface[0], m_surface[1]));
        }
    }
}


void NpapiAsyncDrawService::present(bool initOnly)
{
    if (m_pBufferMutex) {
        m_pBufferMutex->ReleaseSync(0);
        m_pBufferMutex = 0;
    }

    NpapiBrowserHostPtr hostPtr = m_weakHost.lock();
    NpapiBrowserHost* host = hostPtr.get();
    ID3D10Device1* dev = device();
    if (!dev || !host)
        return;

    int next = 1 - m_current;
    m_surface[next].makeCurrent(host);
    int prev = m_current;
    m_current = next;

    // create or (if the size has changed) recreate the back buffer
    if (m_surface[prev].sizeIsDifferent(m_width, m_height)) {
        if (m_width && m_height) {
            NPSize dims = { m_width, m_height };
            m_surface[prev].init(host, &dims);
        }
    }
    
    // send the back buffer to the render thread
    {
        boost::lock_guard<boost::mutex> lock(m_mut);
        REFIID riid = __uuidof(ID3D10Texture2D);
        if (m_surface[prev].valid) {
            HRESULT hr = dev->OpenSharedResource(m_surface[prev].sharedHandle, __uuidof(ID3D10Texture2D), (void**) &m_pBackBuffer.p);
            if (SUCCEEDED(hr)) {
                m_pBackBuffer->QueryInterface(&m_pBufferMutex);
                if (m_pBufferMutex) {
                    hr = m_pBufferMutex->AcquireSync(0, 50);
                    if (FAILED(hr)) {
                        m_pBufferMutex = 0;
                    }
                }

                m_cond.notify_one();
            }
        }
    }
}

NpapiAsyncDrawService::Surface::Surface()
    : valid(false)
{
    version = 0;
}

NPError NpapiAsyncDrawService::Surface::init(NpapiBrowserHost* host, NPSize* size)
{
    if (valid) {
        finalize(host);
    }
    NPError result = host->InitAsyncSurface(size, NPImageFormatBGRA32, NULL /*initData*/, this);
    valid = (0 == result);
    return result;
}

void NpapiAsyncDrawService::Surface::finalize(NpapiBrowserHost* host)
{
    if (valid) {
        host->FinalizeAsyncSurface(this);
        valid = false;
    }
}

void NpapiAsyncDrawService::Surface::makeCurrent(NpapiBrowserHost* host)
{
    host->SetCurrentAsyncSurface(valid ? this : 0, NULL);
}

bool NpapiAsyncDrawService::Surface::sizeIsDifferent(unsigned width, unsigned height) const
{
    return !valid || height != size.height || width != size.width;
}
