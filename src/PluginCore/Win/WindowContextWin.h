/**********************************************************\ 
 Original Author: Georg Fritzsche
 
 Created:    Oct 23, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \*********************************************************
 */

#pragma once
#ifndef H_FB_WINDOWCONTEXTWIN
#define H_FB_WINDOWCONTEXTWIN

#include "win_common.h"
#include "FBPointers.h"

namespace FB
{
    struct WindowContextWin
    {
        HWND handle;
        WindowContextWin(HWND handle) 
          : handle(handle)
        {}
    };

    FB_FORWARD_PTR(AsyncDrawService);

    struct WindowContextWindowless
    {
        HDC drawable;
        AsyncDrawServicePtr asyncDraw;

        WindowContextWindowless(HDC drawable)
            : drawable(drawable)
        {}
        WindowContextWindowless(HDC drawable, AsyncDrawServicePtr p) 
            : drawable(drawable)
            , asyncDraw(p)
        {}
    };

}

#endif

