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
 \**********************************************************/


#pragma once
#ifndef H_FB_PLUGINWINDOWFORWARDDECL
#define H_FB_PLUGINWINDOWFORWARDDECL

#include <boost/shared_ptr.hpp>

#if defined(XP_MACOSX) && defined(__LP64__)
#define NP_NO_QUICKDRAW
#define NP_NO_CARBON
#endif

namespace FB
{
#ifdef FB_WIN
    class PluginWindowWin;
    class PluginWindowlessWin;
    struct WindowContextWin;
    struct WindowContextWindowless;
#endif
    
#ifdef FB_X11
    class PluginWindowX11;
    struct WindowContextX11;
#endif
    
#ifdef FB_MACOSX
    class PluginWindowMacICA;
    class PluginWindowMacCA;
    class PluginWindowMacCG;
    class PluginWindowMacQD;
    class PluginEventMacCocoa;
    class PluginEventMacCarbon;
#endif 
}

#endif

