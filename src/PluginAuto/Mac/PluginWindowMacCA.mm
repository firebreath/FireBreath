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
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/DrawingEvents.h"

#include "PluginWindowMacCA.h"

using namespace FB;

FB::PluginWindowMacCA* FB::createPluginWindowMacCA(bool invalidating)
{
    return new PluginWindowMacCA(invalidating);
}

PluginWindowMacCA::PluginWindowMacCA(bool invalidating)
    : PluginWindowMac(), m_layer(NULL), m_invalidating(invalidating)
{
    CALayer *mlayer = [[CALayer layer] retain];
    mlayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    mlayer.needsDisplayOnBoundsChange = YES;
    m_layer = mlayer;
}

PluginWindowMacCA::~PluginWindowMacCA()
{
    CALayer *mlayer = (CALayer*) m_layer;
    [mlayer autorelease];
    m_layer = NULL;
}

int16_t PluginWindowMacCA::GetValue(NPPVariable variable, void *value) {
    CALayer *mlayer = (CALayer*) m_layer;
    if (NPPVpluginCoreAnimationLayer == variable) {
        *(CALayer**) value = mlayer;
        if (m_invalidating)
            (void) scheduleTimer(60, true);
        FBLOG_INFO("PluginCore", "GetValue(NPPVpluginCoreAnimationLayer)=" << (void*) mlayer);
    }
    return NPERR_NO_ERROR;
}

NPError PluginWindowMacCA::SetWindow(NPWindow* window)
{
    CALayer *mlayer = (CALayer*) m_layer;
    FBLOG_INFO("PluginCore", "PluginWindowMacCA::SetWindow() CALayer=" << (void*) mlayer);
    FBLOG_INFO("PluginCore", " bounds(" << window->x << "," << window->y << ")(" << window->x + window->width << "," << window->y + window->height << ")");
    FBLOG_INFO("PluginCore", "   clip(" << window->clipRect.left << "," << window->clipRect.top << ")(" << window->clipRect.right << "," << window->clipRect.bottom << ")");
    FBLOG_INFO("PluginCore", "  layer(" << mlayer.bounds.origin.x << "," << mlayer.bounds.origin.y << ")(" << mlayer.bounds.origin.x + mlayer.bounds.size.width << "," << mlayer.bounds.origin.y + mlayer.bounds.size.height << ")");

    return PluginWindowMac::SetWindow(window);
}

void PluginWindowMacCA::handleTimerEvent() {
    InvalidateWindow();
    PluginWindowMac::handleTimerEvent();
}
