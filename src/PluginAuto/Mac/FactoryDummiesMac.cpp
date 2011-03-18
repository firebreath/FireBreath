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

// Depending on the plugin config, only a part of the mac window classes are actually built.
// This file takes care of the linking errors.

#include "global/config.h"
#include "ConstructDefaultPluginWindows.h"

#if !FBMAC_USE_INVALIDATINGCOREANIMATION
FB::PluginWindowMacICA* FB::createPluginWindowMacICA()
{
    return 0;
}
#endif

#if !FBMAC_USE_COREANIMATION
FB::PluginWindowMacCA* FB::createPluginWindowMacCA()
{
    return 0;
}
#endif


#if !FBMAC_USE_COREGRAPHICS
FB::PluginWindowMacCG* FB::createPluginWindowMacCG()
{
    return 0;
}
#endif

#if !FBMAC_USE_QUICKDRAW && !defined(NP_NO_QUICKDRAW)
FB::PluginWindowMacQD* FB::createPluginWindowMacQD()
{
    return 0;
}
#endif