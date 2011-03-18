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
#ifndef H_FB_CONSTRUCTDEFAULTWINDOWS
#define H_FB_CONSTRUCTDEFAULTWINDOWS

#include "PluginWindowForwardDecl.h"

namespace FB
{
#ifdef FB_WIN
    FB::PluginWindowWin* createPluginWindowWin(const WindowContextWin&);    
    FB::PluginWindowlessWin* createPluginWindowless(const WindowContextWindowless&);
#endif

#ifdef FB_MACOSX
    FB::PluginWindowMacICA* createPluginWindowMacICA();
    FB::PluginWindowMacCA* createPluginWindowMacCA();
    FB::PluginWindowMacCG* createPluginWindowMacCG();
    FB::PluginWindowMacQD* createPluginWindowMacQD();
    FB::PluginEventMacCocoa* createPluginEventMacCocoa();
    FB::PluginEventMacCarbon* createPluginEventMacCarbon();
#endif
    
#ifdef FB_X11
    FB::PluginWindowX11* createPluginWindowX11(const FB::WindowContextX11&);
#endif

}

#endif

