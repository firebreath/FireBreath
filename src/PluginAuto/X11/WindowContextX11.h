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
#ifndef H_FB_WINDOWCONTEXTX11
#define H_FB_WINDOWCONTEXTX11

#include "global/config.h"
#include "NpapiTypes.h"

#if FB_GUI_DISABLED == 1
typedef long int GdkNativeWindow;
#endif

namespace FB
{
    struct WindowContextX11
    {
        GdkNativeWindow window;
        WindowContextX11(GdkNativeWindow window) 
          : window(window)
        {}
    };
}

#endif

