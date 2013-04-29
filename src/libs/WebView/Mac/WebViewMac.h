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
#include "Mac/PluginWindowMac.h"
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
        virtual ~WebViewMac();

        BEGIN_PLUGIN_EVENT_MAP()
            EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::KeyDownEvent, onKeyDown, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::KeyUpEvent, onKeyUp, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::ResizedEvent, onWindowResized, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::MouseScrollEvent, onMouseScroll, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::MouseEnteredEvent, onMouseEntered, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::MouseExitedEvent, onMouseExited, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::FocusChangedEvent, onFocusChanged, FB::PluginWindowMac)
            EVENTTYPE_CASE(FB::CoreGraphicsDraw, onCoreGraphicsDraw, FB::PluginWindowMac)
        END_PLUGIN_EVENT_MAP()

        virtual void loadHtml(const std::string& html);
        virtual void loadUri(const FB::URI& uri);
        virtual void closePage();

        void DrawToCGContext(CGContext* ctx, const FB::Rect& size);

        virtual bool onKeyDown(FB::KeyDownEvent *evt, FB::PluginWindowMac *);
        virtual bool onKeyUp(FB::KeyUpEvent *evt, FB::PluginWindowMac *);

        virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindowMac *);
        virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindowMac *);
        virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindowMac *);
        virtual bool onMouseScroll(FB::MouseScrollEvent *evt, FB::PluginWindowMac *);
        virtual bool onMouseEntered(FB::MouseEnteredEvent *evt, FB::PluginWindowMac *);
        virtual bool onMouseExited(FB::MouseExitedEvent *evt, FB::PluginWindowMac *);
        
        virtual bool onFocusChanged(FB::FocusChangedEvent *evt, FB::PluginWindowMac *);

        virtual bool onCoreGraphicsDraw(FB::CoreGraphicsDraw *evt, FB::PluginWindowMac *);
        virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindowMac *);
        virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindowMac *);
        virtual bool onWindowResized(FB::ResizedEvent *evt, FB::PluginWindowMac *win);
        
        virtual void onFrameLoaded(JSContextRef jsContext, JSObjectRef window, void* frame);
        virtual void onFrameClosing(void* frame);
        
        virtual FB::BrowserHostPtr getPageHost() {
            return m_host;
        }

        virtual bool doWebViewNavigation(const std::string& url);
        virtual bool doWebViewTitleChanged(const std::string& title);

    private:
        boost::scoped_ptr<WebView_ObjCObjects> o;
        FB::MouseButtonEvent::MouseButton mouseButtonState;
        FB::WebKit::WebKitBrowserHostPtr m_host;
        FB::BrowserHostPtr m_parentHost;
        void* m_layer;
        bool m_isInvalidating;
        FB::PluginWindowMac* m_wnd;
    };
}};

#endif // H_WebViewMac__h_
