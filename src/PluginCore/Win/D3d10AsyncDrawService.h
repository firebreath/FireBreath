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
#ifndef H_FB_D3D10ASYNCDRAWSERVICE
#define H_FB_D3D10ASYNCDRAWSERVICE

#include "win_common.h"
#include <d3d10_1.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include "FBPointers.h"
#include "AsyncDrawService.h"

namespace FB
{
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(D3d10DeviceContext);
    FB_FORWARD_PTR(D3d10AsyncDrawService);

    class D3d10AsyncDrawService
        : public AsyncDrawService
    {
    public:
        typedef boost::function<bool(ID3D10Device1*, ID3D10RenderTargetView*, uint32_t, uint32_t)> RenderCallback;

        D3d10AsyncDrawService(BrowserHostPtr);
        ~D3d10AsyncDrawService();

        void resized(uint32_t width, uint32_t height);
        virtual void render(RenderCallback cb);

    protected:
        virtual void present(bool init) = 0;
        ID3D10Device1* device() const;

        BrowserHostWeakPtr m_weakHost;
        D3d10DeviceContextPtr m_dc;

        unsigned m_width, m_height;
        bool m_dimsChanged;         // since last call to present()

        boost::mutex m_mut;
        boost::condition_variable m_cond;
        CComPtr<ID3D10Texture2D> m_pBackBuffer;
        CComPtr<IDXGIKeyedMutex> m_pBufferMutex;
    };  
}

#endif
