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
#ifndef H_FB_WINDOWCONTEXTMAC
#define H_FB_WINDOWCONTEXTMAC

#include "global/config.h"
#include "NpapiTypes.h"

namespace FB
{
#if FBMAC_USE_QUICKDRAW
    struct WindowContextQuickDraw
    {
        CGrafPtr port;
        long x, y;
        WindowContextQuickDraw(CGrafPtr port, long x, long y) 
          : port(port), x(x), y(y) 
        {}
    };
#endif
    
#if FBMAC_USE_COREGRAPHICS
    struct WindowContextCoreGraphics
    {
        NP_CGContext* context;
        WindowContextCoreGraphics(NP_CGContext* context) 
          : context(context)
        {}
    };
#endif
}

#endif

