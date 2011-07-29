/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#import <AppKit/AppKit.h>

#include "Mac/PluginWindowMac.h"
#include "Mac/PluginWindowMacQD.h"
#include "Mac/PluginWindowMacCG.h"
#include "Mac/PluginWindowMacCA.h"
#include "Mac/PluginWindowMacICA.h"

#include "BasicMediaPlayerPluginMac.h"

@interface MyCAOpenGLLayer : CAOpenGLLayer {
    GLfloat m_angle;
}
@end

@implementation MyCAOpenGLLayer

- (id) init {
    if ([super init]) {
        m_angle = 0;
    }
    return self;
}

- (void)drawInCGLContext:(CGLContextObj)ctx pixelFormat:(CGLPixelFormatObj)pf forLayerTime:(CFTimeInterval)t displayTime:(const CVTimeStamp *)ts {
    m_angle += 1;
    GLsizei width = CGRectGetWidth([self bounds]), height = CGRectGetHeight([self bounds]);
    GLfloat halfWidth = width / 2, halfHeight = height / 2;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScalef(1.0f, -1.0f, 1.0f);
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(m_angle, 0.0, 0.0, 1.0);

    // Clear the stage.
    glClearColor(0.0, 0.0, 0.0, 0.0);        
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw UL Quadrant 25% Red
    glBegin(GL_QUADS);
        glColor4f(1.0, 0.0, 0.0, 0.25);
        glVertex3f(0, 0, -1.0f);
        glVertex3f(halfWidth, 0, -1.0f);
        glVertex3f(halfWidth, halfHeight, -1.0f);
        glVertex3f(0, halfHeight, -1.0f);
    glEnd();
    
    // Draw UR Quadrant 50% Green
    glBegin(GL_QUADS);
        glColor4f(0.0, 1.0, 0.0, 0.5);
        glVertex3f(halfWidth, 0, -1.0f);
        glVertex3f(width, 0, -1.0f);
        glVertex3f(width, halfHeight, -1.0f);
        glVertex3f(halfWidth, halfHeight, -1.0f);
    glEnd();
    
    // Draw LR Quadrant 75% Blue
    glBegin(GL_QUADS);
        glColor4f(0.0, 0.0, 1.0, 0.75);
        glVertex3f(halfWidth, halfHeight, -1.0f);
        glVertex3f(width, halfHeight, -1.0f);
        glVertex3f(width, height, -1.0f);
        glVertex3f(halfWidth, height, -1.0f);
    glEnd();
    
    // Draw Center Quadrant 100% Magenta
    glBegin(GL_QUADS);
        glColor4f(1.0, 0.0, 1.0, 1.0);
        glVertex3f(width / 4, height / 4, -1.0f);
        glVertex3f(3 * width / 4, height / 4, -1.0f);
        glVertex3f(3 * width / 4, 3 * height / 4, -1.0f);
        glVertex3f(width / 4,   3 * height / 4, -1.0f);
    glEnd();
    
    [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
}

@end

BasicMediaPlayerPluginMac::BasicMediaPlayerPluginMac() : m_layer(NULL) {}
 
BasicMediaPlayerPluginMac::~BasicMediaPlayerPluginMac()
{
    if (m_layer) {
        [(CALayer*)m_layer removeFromSuperlayer];
        [(CALayer*)m_layer release];
        m_layer = NULL;
    }
}

bool BasicMediaPlayerPluginMac::onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindowMac* wnd)
{
    if (FB::PluginWindowMac::DrawingModelCoreAnimation == wnd->getDrawingModel() || FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == wnd->getDrawingModel()) {
        // Setup CAOpenGL drawing.
        MyCAOpenGLLayer* layer = [MyCAOpenGLLayer new];
        layer.asynchronous = (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == wnd->getDrawingModel()) ? NO : YES;
        layer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
        layer.needsDisplayOnBoundsChange = YES;
        m_layer = layer;
        if (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == wnd->getDrawingModel())
            wnd->StartAutoInvalidate(1.0/30.0);
        [(CALayer*) wnd->getDrawingPrimitive() addSublayer:layer];
        
        // Draw Label
        CATextLayer* txtlayer = [CATextLayer layer];
        txtlayer.string = (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == wnd->getDrawingModel()) ? @"CoreAnimation (Invalidating)" : @"CoreAnimation";
        txtlayer.fontSize = 14;
        txtlayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
        txtlayer.needsDisplayOnBoundsChange = YES;
        [(CALayer*) wnd->getDrawingPrimitive() addSublayer:txtlayer];
    }
    return BasicMediaPlayerPlugin::onWindowAttached(evt,wnd);
}

bool BasicMediaPlayerPluginMac::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindowMac* wnd)
{
    return BasicMediaPlayerPlugin::onWindowDetached(evt,wnd);
}
#ifndef NP_NO_QUICKDRAW
bool BasicMediaPlayerPluginMac::onDrawQD(FB::QuickDrawDraw *evt, FB::PluginWindowMacQD* wnd)
{
    FB::Rect bounds(evt->bounds);
    FB::Rect clip(evt->clip);
    CGrafPtr port(evt->port);
#if 0
    // Wrap a QuickDraw port with a CoreGraphics context.
    CGContextRef cgContext = NULL;
    OSStatus status = QDBeginCGContext(port, &cgContext);
    printf("%s() QDBeginCGContext=%ld\n", __func__, status);
    if (noErr == status) {
        status = SyncCGContextOriginWithPort(cgContext, port);
        printf("%s() SyncCGContextOriginWithPort=%ld\n", __func__, status);

        // Flip the coordinate space.
        CGContextTranslateCTM(cgContext, 0.0, bounds.bottom - bounds.top);
        CGContextScaleCTM(cgContext, 1.0, -1.0);
        
        DrawCoreGraphics(cgContext, bounds, clip);
        
        //CGContextFlush(cgContext);
        CGContextSynchronize(cgContext);
        status = QDEndCGContext(port, &cgContext);
        printf("%s() QDEndCGContext=%ld\n", __func__, status);
        //QDFlushPortBuffer(port, NULL);
    }
    else
#endif
    {
        short width = bounds.right - bounds.left, height = bounds.bottom - bounds.top;
        short halfWidth = width / 2, halfHeight = height / 2;

        CGrafPtr cur_port = NULL;
        bool swappedport = QDSwapPort(port, &cur_port);
        ::Rect portBounds; (void) GetPortBounds(port, &portBounds);
        
        ::Rect clipRect = { clip.top, clip.left, clip.bottom, clip.right};
        OffsetRect(&clipRect, -bounds.left, -bounds.top);
        ClipRect(&clipRect);

        // Clear the stage.
        RGBColor color0 = { USHRT_MAX, USHRT_MAX, USHRT_MAX };
        RGBBackColor(&color0); //BackColor(whiteColor);
        ::Rect rect0 = { 0, 0, height, width };
//      EraseRect(&rect0);

        // Draw UL Quadrant 25% Red
        RGBColor color1 = { USHRT_MAX, 0, 0 };
        RGBForeColor(&color1); //ForeColor(redColor);
        ::Rect rect1 = { 0, 0, halfHeight, halfWidth };
        PaintRect(&rect1);

        // Draw UR Quadrant 50% Green
        RGBColor color2 = { 0, USHRT_MAX, 0 };
        RGBForeColor(&color2); //ForeColor(greenColor);
        ::Rect rect2 = { 0, halfWidth, halfHeight, width };
        PaintRect(&rect2);

        // Draw LR Quadrant 75% Blue
        RGBColor color3 = { 0, 0, USHRT_MAX };
        RGBForeColor(&color3); //ForeColor(blueColor);
        ::Rect rect3 = { halfHeight, halfWidth, height, width };
        PaintRect(&rect3);

        // Draw Center Quadrant 100% Magenta
        RGBColor color4 = { USHRT_MAX, 0, USHRT_MAX };
        RGBForeColor(&color4); //ForeColor(magentaColor);
        ::Rect rect4 = { halfHeight / 2, halfWidth / 2, 3 * height / 4, 3 * width / 4 };
        PaintRect(&rect4);
        
        // Draw Label
        RGBBackColor(&color1); //BackColor(redColor);
        RGBForeColor(&color0); //ForeColor(whiteColor);
        ::Rect rect5 = { 0, 0, 14, halfWidth };
        const char* label = "QuickDraw";
        TETextBox(label, strlen(label), &rect5, teJustLeft);

        SetOrigin(portBounds.left, portBounds.top);
        if (swappedport) QDSwapPort(port, NULL);
    }
    return true; // We have handled the event
}
#endif
bool BasicMediaPlayerPluginMac::onDrawCG(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG*)
{
    FB::Rect bounds(evt->bounds);
    FB::Rect clip(evt->clip);
    CGContextRef cgContext(evt->context);

#if 0
    // Wrap a CoreGraphics context with a NSGraphics context.
    NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:cgContext flipped:YES];
    if (nsContext)
        DrawNSGraphicsContext(nsContext, bounds, clip);
    else
#endif
    {
        short width = bounds.right - bounds.left, height = bounds.bottom - bounds.top;
        short halfWidth = width / 2, halfHeight = height / 2;

        CGContextSaveGState(cgContext);
        //CGRect clipRect = { {0,0}, {clip.right - clip.left, clip.bottom - clip.top} };
        //CGContextClipToRect(cgContext, clipRect);
        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
        CGContextSetFillColorSpace(cgContext, colorspace);
        
        // Flip the coordinate space.
        //CGContextTranslateCTM(cgContext, 0.0, height);
        //CGContextScaleCTM(cgContext, 1.0, -1.0);

        // Clear the stage.
        CGRect rect0 = { {0,0}, {width, height} };
        //CGContextClearRect(cgContext, rect0);

        // Draw UL Quadrant 25% Red
        CGFloat color1[] = { 1.0, 0.0, 0.0, 0.25 };
        CGContextSetFillColor(cgContext, color1);
        CGRect rect1 = { {0, 0}, {halfWidth, halfHeight} };
        CGContextFillRect(cgContext, rect1);

        // Draw UR Quadrant 50% Green
        CGFloat color2[] = { 0.0, 1.0, 0.0, 0.50 };
        CGContextSetFillColor(cgContext, color2);
        CGRect rect2 = { {halfWidth, 0}, {halfWidth, halfHeight} };
        CGContextFillRect(cgContext, rect2);

        // Draw LR Quadrant 75% Blue
        CGFloat color3[] = { 0.0, 0.0, 1.0, 0.75 };
        CGContextSetFillColor(cgContext, color3);
        CGRect rect3 = { {halfWidth, halfHeight}, {halfWidth, halfHeight} };
        CGContextFillRect(cgContext, rect3);

        // Draw Center Quadrant 100% Magenta
        CGFloat color4[] = { 1.0, 0.0, 1.0, 1.00 };
        CGContextSetFillColor(cgContext, color4);
        CGRect rect4 = { {halfWidth / 2, halfHeight / 2}, {halfWidth, halfHeight} };
        CGContextFillRect(cgContext, rect4);
        
        // Draw Label
        const char* label = "CoreGraphics";
        CGContextSetStrokeColorWithColor(cgContext, CGColorGetConstantColor(kCGColorWhite));
        CGContextSetFillColorWithColor(cgContext, CGColorGetConstantColor(kCGColorWhite));
        CGContextSetTextMatrix(cgContext, CGAffineTransformMake(1.0, 0.0, 0.0, -1.0, 0.0, 0.0)); // Flip the text right-side up.
        CGContextSelectFont(cgContext, "Geneva", 14, kCGEncodingMacRoman);
        CGContextShowTextAtPoint(cgContext, 0, 14, label, strlen(label));
            
        CGColorSpaceRelease(colorspace);
        CGContextRestoreGState(cgContext);
    }
    return true; // This is handled
}
void BasicMediaPlayerPluginMac::DrawNSGraphicsContext(void *context, const FB::Rect& bounds, const FB::Rect& clip) {
    short width = bounds.right - bounds.left, height = bounds.bottom - bounds.top;
    short halfWidth = width / 2, halfHeight = height / 2;

    NSGraphicsContext *nsContext = (NSGraphicsContext *) context;
    NSGraphicsContext *oldContext = [NSGraphicsContext currentContext];
    [NSGraphicsContext setCurrentContext:nsContext];
    [nsContext saveGraphicsState];

    // Clear the stage.
    [[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.00] set];
    NSRect rect0 = NSMakeRect(0, 0, width, height);
    [NSBezierPath fillRect:rect0];

    // Draw UL Quadrant 25% Red
    [[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:0.25] set];
    NSRect rect1 = NSMakeRect(0, 0, halfWidth, halfHeight);
    [NSBezierPath fillRect:rect1];

    // Draw UR Quadrant 50% Green
    [[NSColor colorWithCalibratedRed:0.0 green:1.0 blue:0.0 alpha:0.50] set];
    NSRect rect2 = NSMakeRect(halfWidth, 0, halfWidth, halfHeight);
    [NSBezierPath fillRect:rect2];
    
    // Draw LR Quadrant 75% Blue
    [[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:1.0 alpha:0.75] set];
    NSRect rect3 = NSMakeRect(halfWidth, halfHeight, halfWidth, halfHeight);
    [NSBezierPath fillRect:rect3];
    
    // Draw Center Quadrant 100% Magenta
    [[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:1.0 alpha:1.00] set];
    NSRect rect4 = NSMakeRect(halfWidth / 2, halfHeight / 2, halfWidth, halfHeight);
    [NSBezierPath fillRect:rect4];

    // Draw Label
    NSString *label = @"NSGraphicsContext";
    [label drawInRect:rect1 withAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont fontWithName:@"Geneva" size:14], NSFontAttributeName, [NSColor whiteColor], NSForegroundColorAttributeName, NULL]];

    [nsContext restoreGraphicsState];
    [NSGraphicsContext setCurrentContext:oldContext];
}
