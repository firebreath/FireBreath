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

#include "PluginWindowMacCA.h"

using namespace FB;

@interface MyCALayer : CALayer
{
}

@end

@implementation MyCALayer

- (void)dealloc {
    FBLOG_DEBUG("PluginCore", FBLOG_FUNCTION());
    [super dealloc];
}

@end



FB::PluginWindowMacCA* FB::createPluginWindowMacCA()
{
    return new PluginWindowMacCA();
}

PluginWindowMacCA::PluginWindowMacCA()
    : PluginWindowMac(), m_layer(NULL)
{
    MyCALayer *mlayer = [[MyCALayer layer] retain];
    mlayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    mlayer.needsDisplayOnBoundsChange = YES;
    m_layer = mlayer;
}

PluginWindowMacCA::~PluginWindowMacCA()
{
    MyCALayer *mlayer = (MyCALayer*) m_layer;
    [mlayer release];
    m_layer = NULL;
}

int16_t PluginWindowMacCA::GetValue(NPPVariable variable, void *value) {
    MyCALayer *mlayer = (MyCALayer*) m_layer;
    if (NPPVpluginCoreAnimationLayer == variable) {
        // Need to only retain when Safari is run in 32bit mode and is the host process. Backwards compatiblity.
        // When Safari is run in 64bit mode, the plugin is sandboxed in another process. According to curr spec.
        if (NSOrderedSame == [[[NSBundle mainBundle] bundleIdentifier] compare:@"com.apple.Safari"])
            *(CALayer**) value = [mlayer retain];
        else
            *(CALayer**) value = mlayer;
        FBLOG_INFO("PluginCore", "GetValue(NPPVpluginCoreAnimationLayer)=" << (void*) mlayer);
    }
    return NPERR_NO_ERROR;
}

NPError PluginWindowMacCA::SetWindow(NPWindow* window)
{
    MyCALayer *mlayer = (MyCALayer*) m_layer;
    FBLOG_INFO("PluginCore", "PluginWindowMacCA::SetWindow() CALayer=" << (void*) mlayer);
    FBLOG_INFO("PluginCore", " bounds(" << window->x << "," << window->y << ")(" << window->x + window->width << "," << window->y + window->height << ")");
    FBLOG_INFO("PluginCore", "   clip(" << window->clipRect.left << "," << window->clipRect.top << ")(" << window->clipRect.right << "," << window->clipRect.bottom << ")");
    FBLOG_INFO("PluginCore", "  layer(" << mlayer.bounds.origin.x << "," << mlayer.bounds.origin.y << ")(" << mlayer.bounds.origin.x + mlayer.bounds.size.width << "," << mlayer.bounds.origin.y + mlayer.bounds.size.height << ")");

    return PluginWindowMac::SetWindow(window);
}