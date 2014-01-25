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
#ifndef H_FB_D3D10DEVICECONTEXT
#define H_FB_D3D10DEVICECONTEXT

#include "win_common.h"
#include <d3d10_1.h>

namespace FB
{
    class D3d10DeviceContext
    {
    public:
        virtual ~D3d10DeviceContext() {}
        virtual CComPtr<ID3D10Device1> device() = 0;
    };
}

#endif
