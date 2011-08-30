/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Jun 23, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/

#import <WebKit/WebKit.h>
#include "logging.h"
#include "DOM.h"

#import "WebViewMac.h"

#define OFFSCREEN_ORIGIN_X -4000
#define OFFSCREEN_ORIGIN_Y -4000

@implementation WebViewHelper

- (void)setController:(FB::View::WebViewMac*)c
{
    controller = c;
}

- (WebView*)webView {
    return webView;
}

- (NSGraphicsContext*)context {
    return windowContext;
}

- (NSWindow*)hiddenWindow {
    return hiddenWindow;
}

- (id)initWithFrame:(NSRect)frameRect {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSRect windowRect = NSMakeRect(OFFSCREEN_ORIGIN_X, OFFSCREEN_ORIGIN_Y, frameRect.size.width, frameRect.size.height);
    hiddenWindow = [[FBViewWebViewWindow alloc]
        initWithContentRect:windowRect 
                  styleMask:NSBorderlessWindowMask
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [hiddenWindow setIsActive:YES];
    //[hiddenWindow makeKeyAndOrderFront:self];
    [hiddenWindow setAcceptsMouseMovedEvents:YES];
    [hiddenWindow setIgnoresMouseEvents:NO];

    webView = [[WebView alloc] initWithFrame:frameRect frameName:nil groupName: nil];
    [webView setFrameLoadDelegate:self];
    [hiddenWindow setContentView:webView];
    windowContext = [[NSGraphicsContext graphicsContextWithWindow:hiddenWindow] retain];
    [hiddenWindow makeFirstResponder:hiddenWindow.contentView];

    mainFrame = [webView mainFrame];
    jsWindow = [webView windowScriptObject];
    [pool release];
    return self;
}

- (void)loadHTML:(NSString*)html baseUrl:(NSURL*)baseUrl {
    [mainFrame loadHTMLString:html baseURL:baseUrl];
}

- (void)loadURL:(NSURL*)url {
    NSURLRequest *req = [[NSURLRequest alloc] initWithURL:url];

    [mainFrame loadRequest:req];

    [req release];
}

- (void)drawToCGContext:(CGContextRef)ctx asRect:(NSRect)newSize
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    //    //You could do this to get the bits instead of drawing directly to the context:
    //    [webView lockFocus];
    //    NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc] initWithFocusedViewRect:[webView bounds]] autorelease];
    //    [webView unlockFocus];

    NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:gc];

    CGContextTranslateCTM(ctx, 0.0, newSize.size.height);
    CGContextScaleCTM(ctx, 1.0, -1.0);
    
    [webView displayRectIgnoringOpacity:newSize inContext:gc];

    [NSGraphicsContext restoreGraphicsState];
    [pool release];
}

- (void)webView:(WebView *)sender didClearWindowObject:(WebScriptObject *)windowObject forFrame:(WebFrame *)frame
{
    NSString* name = [frame name];
    std::string str = [name UTF8String];
    if (controller && str.empty())
        controller->onFrameLoaded([frame globalContext], [windowObject JSObject], frame);
}

- (void)webView:(WebView *)sender willCloseFrame:(WebFrame *)frame
{
    NSString* name = [frame name];
    std::string str = [name UTF8String];
    if (str.empty()) 
        controller->onFrameClosing(frame);
}

- (void)dealloc {
    FBLOG_DEBUG("WebViewHelper", FBLOG_FUNCTION());
    [webView release];
    [hiddenWindow release];
    [super dealloc];
}

@end

void FB::View::WebViewMac::onFrameLoaded(JSContextRef jsContext, JSObjectRef window, void* frame)
{
    m_host = boost::make_shared<FB::WebKit::WebKitBrowserHost>(jsContext, window, m_parentHost);
    FB::DOM::WindowPtr wnd = m_host->getDOMWindow();
    
    FB::VariantMap& injectVars(getJSVariables());
    FB::VariantMap::iterator it(injectVars.begin());
    FB::VariantMap::iterator end(injectVars.end());
    while (it != injectVars.end()) {
        wnd->setProperty(it->first, it->second);
        ++it;
    }
}
void FB::View::WebViewMac::onFrameClosing(void* frame)
{
    if (m_host) {
        // Close the BrowserHost for the page that is closing
        m_host->shutdown();
        m_host.reset();
    }
}

FB::View::WebViewPtr FB::View::WebView::create( const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost )
{
    FB::View::WebViewPtr ptr(boost::make_shared<FB::View::WebViewMac>(plugin, parentHost));
    ptr->init();
    return ptr;
}

FB::View::WebViewMac::WebViewMac(const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost)
    : FB::View::WebView(plugin, parentHost), o(new WebView_ObjCObjects()), mouseButtonState(FB::MouseButtonEvent::MouseButton_None),
      m_parentHost(parentHost)
{
}

FB::View::WebViewMac::~WebViewMac() {
}

void FB::View::WebViewMac::loadHtml(const std::string& html)
{
    NSString *nsHtml = [[NSString alloc] initWithCString:html.data()];
    NSURL *nsBaseUrl = [[NSURL alloc] initWithString:@"http://www.google.com"];
    
    [o->helper loadHTML:nsHtml baseUrl:nsBaseUrl];
    
    [nsBaseUrl release];
    [nsHtml release];
}

void FB::View::WebViewMac::loadUri(const FB::URI& uri)
{
    NSString *nsUrlStr = [[NSString alloc] initWithCString:uri.toString().data()];
    NSURL *nsUrl = [[NSURL alloc] initWithString:nsUrlStr];
    
    [o->helper loadURL:nsUrl];
    
    [nsUrl release];
    [nsUrlStr release];
}

void FB::View::WebViewMac::closePage()
{
    NSURL *nsUrl = [[NSURL alloc] initWithString:@"about:blank"];
    
    [o->helper loadURL:nsUrl];
    
    [nsUrl release];
}

void FB::View::WebViewMac::DrawToCGContext(CGContext* ctx, const FB::Rect& size)
{
    NSAutoreleasePool * pool = [NSAutoreleasePool new];
    NSRect newSize = NSMakeRect(0, 0, size.right-size.left, size.bottom-size.top);
    
    [o->helper drawToCGContext:ctx asRect:newSize];
    
    [pool release];
}

bool FB::View::WebViewMac::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindowMacCG *wnd)
{
    NSRect frame = NSMakeRect(0, 0, wnd->getWindowWidth(), wnd->getWindowHeight());
    o->helper = [[WebViewHelper alloc] initWithFrame:frame];
    [o->helper setController:this];    
    
    wnd->StartAutoInvalidate(1.0/30.0);
    return false;
}

bool FB::View::WebViewMac::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindowMacCG *wnd)
{
    wnd->StopAutoInvalidate();
    [o->helper release];
    o->helper = nil;
    return false;
}

bool FB::View::WebViewMac::onWindowResized(FB::ResizedEvent *evt, FB::PluginWindowMacCG *win)
{
    if (o->helper) {
        FB::Rect size = win->getWindowPosition();
        NSRect newSize = NSMakeRect(0, 0, size.right-size.left, size.bottom-size.top);
        [o->helper.hiddenWindow setFrame:newSize display:YES];
    }
    
    return true;
}

bool FB::View::WebViewMac::onKeyDown(FB::KeyDownEvent *evt, FB::PluginWindowMacCG *)
{
    NSEventType evtType = NSKeyDown;
    
    int modifierFlags = evt->m_modifierFlags;
    unichar key = evt->m_os_key_code;
    
    NSString *eventChar = [NSString stringWithCharacters:&key length:1];
    NSString *charBezMod = eventChar;
    
    NSEvent *keyEvt = [NSEvent keyEventWithType:evtType
                                       location:NSMakePoint(5, 5)
                                  modifierFlags:modifierFlags
                                      timestamp:GetCurrentEventTime()
                                   windowNumber:[o->helper.hiddenWindow windowNumber]
                                        context:[o->helper context]
                                     characters:eventChar
                    charactersIgnoringModifiers:charBezMod
                                      isARepeat:NO
                                        keyCode:0];

    [o->helper.hiddenWindow.firstResponder keyDown:keyEvt];
    
    return true;
}
bool FB::View::WebViewMac::onKeyUp(FB::KeyUpEvent *evt, FB::PluginWindowMacCG *)
{
    NSEventType evtType = NSKeyUp;
    
    int modifierFlags = evt->m_modifierFlags;
    unichar key = evt->m_os_key_code;
    
    NSString *eventChar = [NSString stringWithCharacters:&key length:1];
    NSString *charBezMod = eventChar;
    
    NSEvent *keyEvt = [NSEvent keyEventWithType:evtType
                                       location:NSMakePoint(5, 5)
                                  modifierFlags:modifierFlags
                                      timestamp:GetCurrentEventTime()
                                   windowNumber:[o->helper.hiddenWindow windowNumber]
                                        context:[o->helper context]
                                     characters:eventChar
                    charactersIgnoringModifiers:charBezMod
                                      isARepeat:NO
                                        keyCode:0];

    [o->helper.hiddenWindow.firstResponder keyUp:keyEvt];

    return true;
}

bool FB::View::WebViewMac::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindowMacCG *wnd)
{
    NSPoint where;
    where.x = evt->m_x;
    where.y = wnd->getWindowHeight()-evt->m_y;
    
//    NSView* resp = [[o->helper.hiddenWindow contentView] hitTest:where];
    
    NSEventType evtType;
    
//    std::stringstream ss;
//    ss << "Mouse down at " << where.x << ", " << where.y;
//    getParentHost()->htmlLog(ss.str());
    mouseButtonState = evt->m_Btn;
    switch (evt->m_Btn) {
        case FB::MouseButtonEvent::MouseButton_Left:
            evtType = NSLeftMouseDown;
            break;
        case FB::MouseButtonEvent::MouseButton_Right:
            evtType = NSRightMouseDown;
            break;
        default:
            break;
    }
    
//    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    NSEvent *mouseDown = [NSEvent mouseEventWithType:evtType
                                            location:where
                                       modifierFlags:nil
                                           timestamp:GetCurrentEventTime()
                                        windowNumber:[o->helper.hiddenWindow windowNumber]
                                             context:[o->helper context]
                                         eventNumber:nil
                                          clickCount:1 
                                            pressure:nil];

    //NSLog(@"%@", o->helper.hiddenWindow.firstResponder);
    [o->helper.hiddenWindow.firstResponder mouseDown:mouseDown];
    wnd->InvalidateWindow();
    return true;
}
bool FB::View::WebViewMac::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindowMacCG *wnd)
{
    NSPoint where;
    where.x = evt->m_x;
    where.y = wnd->getWindowHeight()-evt->m_y;
    
//    NSView* resp = [[o->helper.hiddenWindow contentView] hitTest:where];
    NSEventType evtType;
    
//    std::stringstream ss;
//    ss << "Mouse up at " << where.x << ", " << where.y;
//    getParentHost()->htmlLog(ss.str());
    
    mouseButtonState = FB::MouseButtonEvent::MouseButton_None;
    switch (evt->m_Btn) {
        case FB::MouseButtonEvent::MouseButton_Left:
            evtType = NSLeftMouseUp;
            break;
        case FB::MouseButtonEvent::MouseButton_Right:
            evtType = NSRightMouseUp;
            break;
        default:
            break;
    }
    
//    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    NSEvent *mouseEvent = [NSEvent mouseEventWithType:evtType
                                             location:where
                                        modifierFlags:nil
                                            timestamp:GetCurrentEventTime()
                                         windowNumber:[o->helper.hiddenWindow windowNumber]
                                              context:[o->helper context]
                                          eventNumber:nil
                                           clickCount:1 
                                             pressure:nil];
//    NSLog(@"%@", o->helper.hiddenWindow.firstResponder);
    [o->helper.hiddenWindow.firstResponder mouseUp:mouseEvent];
    wnd->InvalidateWindow();
    return true;
}
bool FB::View::WebViewMac::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindowMacCG *wnd)
{
    NSPoint where;
    where.x = evt->m_x;
    where.y = wnd->getWindowHeight()-evt->m_y;
    
    NSEventType evtType;
    //NSView* resp = [[o->helper.hiddenWindow contentView] hitTest:where];
    
    if (mouseButtonState == FB::MouseButtonEvent::MouseButton_Left) {
        evtType = NSLeftMouseDragged;
    } else if (mouseButtonState == FB::MouseButtonEvent::MouseButton_Right) {
        evtType = NSRightMouseDragged;
    } else {
        evtType = NSMouseMoved;
    }
    
//    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    NSEvent *mouseEvent = [NSEvent mouseEventWithType:evtType
                                             location:where
                                        modifierFlags:nil
                                            timestamp:GetCurrentEventTime()
                                         windowNumber:[o->helper.hiddenWindow windowNumber]
                                              context:[o->helper context]
                                          eventNumber:nil
                                           clickCount:0
                                             pressure:nil];
    
    //NSLog(@"%@", o->helper.hiddenWindow.firstResponder);
//    std::stringstream ss;
    if (evtType == NSMouseMoved) {
        [o->helper.hiddenWindow.firstResponder mouseMoved:mouseEvent];
        [o->helper.hiddenWindow.firstResponder mouseDragged:mouseEvent];
//        ss << "Mouse moved at " << where.x << ", " << where.y;
    } else {
        [o->helper.hiddenWindow.firstResponder mouseDragged:mouseEvent];
//        ss << "Mouse dragged at " << where.x << ", " << where.y;
    }
//    getParentHost()->htmlLog(ss.str());
    
    wnd->InvalidateWindow();
    return false;
}

bool FB::View::WebViewMac::onMouseScroll(FB::MouseScrollEvent *evt, FB::PluginWindowMacCG *wnd) {
    NSPoint where;
    where.x = evt->m_x;
    where.y = wnd->getWindowHeight()-evt->m_y;    
    
    CGWheelCount wheelCount = 2; // 1 for Y-only, 2 for Y-X, 3 for Y-X-Z
    int32_t xScroll = evt->m_dx; // Negative for right
    int32_t yScroll = evt->m_dy; // Negative for down
    CGEventRef cgEvent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, wheelCount, yScroll, xScroll);
    NSScreen* mainScreen = [NSScreen mainScreen];
    CGPoint location = CGPointMake(where.x, mainScreen.frame.size.height - where.y);
    CGEventSetLocation(cgEvent, location);
    
    NSEvent *scrollEvent = [NSEvent eventWithCGEvent:cgEvent];
    
    NSLog(@"%@", o->helper.hiddenWindow.firstResponder);
    [o->helper.hiddenWindow.firstResponder scrollWheel:scrollEvent]; 
    CFRelease(cgEvent);
    
    return true;
}
bool FB::View::WebViewMac::onMouseEntered(FB::MouseEnteredEvent *evt, FB::PluginWindowMacCG *wnd) {
//    NSPoint where;
//    where.x = evt->m_x;
//    where.y = wnd->getWindowHeight()-evt->m_y;
//    NSView* resp = [[o->helper.hiddenWindow contentView] hitTest:where];
//    NSEvent *e = [NSEvent enterExitEventWithType:NSMouseEntered
//                                        location:where
//                                   modifierFlags:nil
//                                       timestamp:GetCurrentEventTime()
//                                    windowNumber:[o->helper.hiddenWindow windowNumber]
//                                         context:[o->helper context]
//                                     eventNumber:nil
//                                  trackingNumber:0
//                                        userData:nil];
//    [o->helper.hiddenWindow.firstResponder mouseEntered:e];
    
    return true;
}
bool FB::View::WebViewMac::onMouseExited(FB::MouseExitedEvent *evt, FB::PluginWindowMacCG *wnd) {
//    NSPoint where;
//    where.x = evt->m_x;
//    where.y = wnd->getWindowHeight()-evt->m_y;
//    NSView* resp = [[o->helper.hiddenWindow contentView] hitTest:where];
//    NSEvent *e = [NSEvent enterExitEventWithType:NSMouseExited
//                                        location:where
//                                   modifierFlags:nil
//                                       timestamp:GetCurrentEventTime()
//                                    windowNumber:[o->helper.hiddenWindow windowNumber]
//                                         context:[o->helper context]
//                                     eventNumber:nil
//                                  trackingNumber:0
//                                        userData:nil];
//    [o->helper.hiddenWindow.firstResponder mouseExited:e];
    
    return true;
}

bool FB::View::WebViewMac::onFocusChanged(FB::FocusChangedEvent *evt, FB::PluginWindowMacCG *)
{
    [o->helper.hiddenWindow setIsActive:evt->hasFocus() ? YES : NO];
    [o->helper.hiddenWindow.contentView setNeedsDisplay:YES];
    return true;
}

bool FB::View::WebViewMac::onCoreGraphicsDraw(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG *wnd)
{
    CGContextSaveGState(evt->context);
    DrawToCGContext(evt->context, evt->bounds);
    CGContextRestoreGState(evt->context);
    
    return true;
}

