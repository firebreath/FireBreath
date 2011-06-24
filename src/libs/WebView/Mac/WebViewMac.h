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

#ifndef H_WebViewMac__h_
#define H_WebViewMac__h_

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
#include <Carbon/Carbon.h>
#include "APITypes.h"
#include "PluginEventSink.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/DrawingEvents.h"
#include "Mac/PluginWindowMacCG.h"
#include "WebKitBrowserHost.h"
#include "URI.h"
#include "WebView.h"

namespace FB { 
    FB_FORWARD_PTR(PluginCore);
    namespace View {
    FB_FORWARD_PTR(WebViewMac);
}};

#ifdef __OBJC__
@interface FBViewWebViewWindow : NSPanel {
    BOOL isActive;
}
- (BOOL)worksWhenModal;
- (void)setIsActive:(BOOL)active;
@end
@implementation FBViewWebViewWindow

- (void)setIsActive:(BOOL)active
{
    isActive = active;
}

- (BOOL)worksWhenModal
{
    return YES;
}

- (BOOL)canBecomeKeyWindow
{
    return isActive;
}

- (BOOL)isKeyWindow
{
    return isActive;
}

//- (BOOL)isMainWindow
//{
//    return YES;
//}
//
- (BOOL)acceptsMouseMovedEvents
{
    return YES;
}

- (BOOL)ignoresMouseEvents
{
    return NO;
}

@end


@class WebView, WebFrame, NSGraphicsContext;
@interface WebViewHelper : NSObject
{
    WebView* webView;
    WebFrame* mainFrame;
    FBViewWebViewWindow* hiddenWindow;
    NSGraphicsContext* windowContext;
    
    FB::View::WebViewMac* controller;

    id jsWindow;
}
- (void)setController:(FB::View::WebViewMac*)c;
@end
namespace FB { namespace View {
    struct WebView_ObjCObjects {
        WebViewHelper* helper;
    };
}}
#else
namespace FB { namespace View {
    struct WebView_ObjCObjects;
}}
#endif

namespace FB { namespace View {
    class WebViewMac : public WebView
    {
    public:
        WebViewMac(const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost);
        ~WebViewMac();
        
        void loadHtml(const std::string& html);
        void loadUri(const FB::URI& uri);
        void closePage();

        void DrawToCGContext(CGContext* ctx, const FB::Rect& size);

        BEGIN_PLUGIN_EVENT_MAP()
            EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::KeyDownEvent, onKeyDown, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::KeyUpEvent, onKeyUp, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::ResizedEvent, onWindowResized, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::MouseScrollEvent, onMouseScroll, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::MouseEnteredEvent, onMouseEntered, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::MouseExitedEvent, onMouseExited, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::FocusChangedEvent, onFocusChanged, FB::PluginWindowMacCG)
            EVENTTYPE_CASE(FB::CoreGraphicsDraw, onCoreGraphicsDraw, FB::PluginWindowMacCG)
        END_PLUGIN_EVENT_MAP()

        virtual bool onKeyDown(FB::KeyDownEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onKeyUp(FB::KeyUpEvent *evt, FB::PluginWindowMacCG *);

        virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onMouseScroll(FB::MouseScrollEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onMouseEntered(FB::MouseEnteredEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onMouseExited(FB::MouseExitedEvent *evt, FB::PluginWindowMacCG *);
        
        virtual bool onFocusChanged(FB::FocusChangedEvent *evt, FB::PluginWindowMacCG *);

        virtual bool onCoreGraphicsDraw(FB::CoreGraphicsDraw *evt, FB::PluginWindowMacCG *);
        virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindowMacCG *);
        virtual bool onWindowResized(FB::ResizedEvent *evt, FB::PluginWindowMacCG *win);
        
        virtual void onFrameLoaded(JSContextRef jsContext, JSObjectRef window, void* frame);
        virtual void onFrameClosing(void* frame);
        
        virtual FB::BrowserHostPtr getPageHost() {
            return m_host;
        }

    private:
        boost::scoped_ptr<WebView_ObjCObjects> o;
        FB::MouseButtonEvent::MouseButton mouseButtonState;
        FB::WebKit::WebKitBrowserHostPtr m_host;
        FB::BrowserHostPtr m_parentHost;
    };
}};

#endif // H_WebViewMac__h_
