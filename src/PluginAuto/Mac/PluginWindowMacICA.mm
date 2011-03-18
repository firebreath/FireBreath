/**********************************************************\
Original Author: Anson MacKeracher
 
Created:    May 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
 
Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "ConstructDefaultPluginWindows.h"

#include "PluginWindowMacICA.h"

using namespace FB;

FB::PluginWindowMacICA* FB::createPluginWindowMacICA()
{
    return new PluginWindowMacICA();
}

PluginWindowMacICA::PluginWindowMacICA()
    : PluginWindowMacCA(), m_displayOnInvalidate(true)
{
}

void PluginWindowMacICA::InvalidateWindow() const {
    if (m_displayOnInvalidate) {
        CALayer *mlayer = (CALayer*) m_layer;
#if 1
        // setNeedsDisplay will draw sometime in the future.
        [[mlayer sublayers] makeObjectsPerformSelector:@selector(setNeedsDisplay)];
#else
        // display will draw now, immediately.
        [[mlayer sublayers] makeObjectsPerformSelector:@selector(display)];
#endif
    }
    PluginWindowMac::InvalidateWindow();
}
