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
    FBLOG_DEBUG("PluginCore", "");
    [super dealloc];
}

@end


FB::PluginWindowMacCA* FB::createPluginWindowMacCA()
{
    return new PluginWindowMacCA();
}

PluginWindowMacCA::PluginWindowMacCA()
    : PluginWindowMac(), m_layer(NULL), m_txtlayer(NULL)
{
    MyCALayer *mlayer = [[MyCALayer layer] retain];
    mlayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    mlayer.needsDisplayOnBoundsChange = YES;
    m_layer = mlayer;

    CATextLayer* txtlayer = [[CATextLayer layer] retain];
    txtlayer.string = (DrawingModelCoreAnimation == getDrawingModel()) ? @"CoreAnimation" : @"InvalidatingCoreAnimation";
    txtlayer.fontSize = 24;
    txtlayer.foregroundColor = CGColorCreateGenericRGB(0.0, 1.0, 0.0, 1.0);
    txtlayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    txtlayer.needsDisplayOnBoundsChange = YES;
    txtlayer.hidden = true;
    [mlayer addSublayer:txtlayer];
    m_txtlayer = txtlayer;
}

PluginWindowMacCA::~PluginWindowMacCA()
{
    CATextLayer *txtlayer = (CATextLayer*) m_txtlayer;
    [txtlayer release];
    m_txtlayer = NULL;

    MyCALayer *mlayer = (MyCALayer*) m_layer;
    [mlayer release];
    m_layer = NULL;
}

// Whether the browser expects a non-retained Core Animation layer.
static const unsigned WKNVExpectsNonretainedLayer = 74657;


int16_t PluginWindowMacCA::GetValue(NPPVariable variable, void *value) {
    MyCALayer *mlayer = (MyCALayer*) m_layer;
    if (NPPVpluginCoreAnimationLayer == variable) {
        NSBundle* mainb = [NSBundle mainBundle];
        // Check the undocumented WKNVExpectsNonretainedLayer added to WebKit2 June 4.
        // Use the supported response as definitive. Not supported as of Firefox 7.0.0.
        NPBool expectsNonretainedLayer = false;
        if (NPERR_NO_ERROR == m_npHost->GetValue((NPNVariable) WKNVExpectsNonretainedLayer, &expectsNonretainedLayer))
            *(CALayer**) value = (expectsNonretainedLayer) ? mlayer : [mlayer retain];
        // Need to only retain when Safari is run in 32bit mode and is the host process. Backwards compatiblity.
        // When Safari is run in 64bit mode, the plugin is sandboxed in another process. According to curr spec.
        // Also, allow the application to specify this behavior in their main bundle plist file, in case it's
        // an application linking to WebKit in 32bit mode, but it's not called Safari
        else if (NSOrderedSame == [[mainb bundleIdentifier] compare:@"com.apple.Safari"] ||
            [mainb objectForInfoDictionaryKey:@"FirebreathCALayerRetain"])
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
    CATextLayer* txtlayer = (CATextLayer*) m_txtlayer;
    FBLOG_INFO("PluginCore", "CALayer=" << (void*) mlayer);
    FBLOG_INFO("PluginCore", " bounds(" << window->x << "," << window->y << ")(" << window->x + window->width << "," << window->y + window->height << ")");
    FBLOG_INFO("PluginCore", "   clip(" << window->clipRect.left << "," << window->clipRect.top << ")(" << window->clipRect.right << "," << window->clipRect.bottom << ")");
    FBLOG_INFO("PluginCore", "  layer(" << mlayer.bounds.origin.x << "," << mlayer.bounds.origin.y << ")(" << mlayer.bounds.origin.x + mlayer.bounds.size.width << "," << mlayer.bounds.origin.y + mlayer.bounds.size.height << ")");

    NPError rval = PluginWindowMac::SetWindow(window);
    txtlayer.hidden = !m_drawLabel;
    [txtlayer removeFromSuperlayer];
    [mlayer addSublayer:txtlayer];
    return rval;
}

