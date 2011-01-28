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

#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginWindowMacCocoaCA.h"
#include "ConstructDefaultPluginWindows.h"

#import <QuartzCore/CALayer.h>

using namespace FB;

FB::PluginWindowMacCocoaCA* FB::createPluginWindowCocoaCA()
{
    return new PluginWindowMacCocoaCA();
}

struct FB::PluginWindowMacCocoaCA::Impl
{
    CALayer *layer;
    Impl(CALayer *layer) : layer(layer) {}
};

PluginWindowMacCocoaCA::PluginWindowMacCocoaCA()
  : m_impl(new Impl([CALayer new]))
{
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0; 
}

PluginWindowMacCocoaCA::~PluginWindowMacCocoaCA() {
    this->clearWindow();
}

void PluginWindowMacCocoaCA::clearWindow() {
    // TODO
}

void* PluginWindowMacCocoaCA::getLayer() const
{
    return static_cast<void*>(m_impl->layer);
}

void PluginWindowMacCocoaCA::setLayer(void* layer) {
    m_impl->layer = static_cast<CALayer*>(layer);
}

void PluginWindowMacCocoaCA::InvalidateWindow() const {
    // Force a drawInContext message to be passed to the window's CALayer
    [m_impl->layer setNeedsDisplay];
}
