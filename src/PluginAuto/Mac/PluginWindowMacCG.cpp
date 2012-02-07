/**********************************************************\
Original Author: Georg Fritzsche

Created:    Mar 26, 2010
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
#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/MacEventCarbon.h"
#include "Mac/PluginWindowMacCG.h"

#include "PluginWindowMacCG.h"
#include <Carbon/Carbon.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1050
typedef float CGFloat; // hack for 10.4 sdk
#endif

using namespace FB;

FB::PluginWindowMacCG* FB::createPluginWindowMacCG()
{
    return new PluginWindowMacCG();
}

PluginWindowMacCG::PluginWindowMacCG()
    : PluginWindowMac(), m_cgContext(), m_count(0)
{
}

PluginWindowMacCG::~PluginWindowMacCG()
{
}

NPError PluginWindowMacCG::SetWindow(NPWindow* window)
{
    NP_CGContext *context = (NP_CGContext*) window->window;
//  FBLOG_TRACE("PluginCore", "PluginWindowMacCG::SetWindow() NP_CGContext=%p", context);
    if (context) { // This is all irrelevant with cocoa; context will be NULL
        if ((m_cgContext.context != context->context)
            || (m_cgContext.window != context->window))
        {
            m_cgContext = *context;
            m_x = window->x;
            m_y = window->y;
            m_width = window->width;
            m_height = window->height;
            m_clipTop = window->clipRect.top;
            m_clipLeft = window->clipRect.left;
            m_clipBottom = window->clipRect.bottom;
            m_clipRight = window->clipRect.right;

            ChangedEvent evt;
            SendEvent(&evt);
        }
    }
    return PluginWindowMac::SetWindow(window);
}

void PluginWindowMacCG::DrawLabel(CGContextRef cgContext, FB::Rect bounds) {
    UnsignedWide currentTime; Microseconds(&currentTime);
    m_frames.push_front(UnsignedWideToUInt64(currentTime));
    if (m_frames.size() > 100) m_frames.pop_back(); // only keep the last 100 frames.
    float fps = m_frames.size() / ((m_frames.front() - m_frames.back()) / 1000000.0);

    CGContextSaveGState(cgContext);
    CGColorSpaceRef rgbSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

    CGFloat boxColorValues[4] = {0.0, 0.0, 0.0, 1.0};
    CGColorRef boxColor = CGColorCreate(rgbSpace, boxColorValues);
    CGContextSetFillColorWithColor(cgContext, boxColor);
    CGContextFillRect(cgContext, CGRectMake(0, 0, 400, 24));
    CGColorRelease(boxColor);

    char label[256];
    sprintf(label, "CoreGraphics %.01f fps #%lld", fps, m_count++);
    CGFloat txtColorValues[4] = {0.0, 1.0, 0.0, 1.0};
    CGColorRef txtColor = CGColorCreate(rgbSpace, txtColorValues);
    CGContextSetStrokeColorWithColor(cgContext, txtColor);
    CGContextSetFillColorWithColor(cgContext, txtColor);
    CGColorRelease(txtColor);
    CGColorSpaceRelease(rgbSpace);
    CGContextSetTextMatrix(cgContext, CGAffineTransformMake(1.0, 0.0, 0.0, -1.0, 0.0, 0.0)); // Flip the text right-side up.
    CGContextSelectFont(cgContext, "Helvetica", 24, kCGEncodingMacRoman);
    CGContextShowTextAtPoint(cgContext, 0, 19, label, strlen(label));

    CGContextRestoreGState(cgContext);
}

bool PluginWindowMacCG::SendEvent(PluginEvent* evt) {
    // Let the plugin have first shot at drawing.
    bool rval = PluginWindowMac::SendEvent(evt);
    // Now, if we are supposed to draw the label...
    if (m_drawLabel) {
        // Firebreath gives plugins the chance to handle raw MacEventCocoa and MacEventCarbon events first.
        // So if we have a CoreGraphicsDraw then the plugin didn't say they processed the raw event.
        if (evt->validType<CoreGraphicsDraw>()) {
            FB::CoreGraphicsDraw *event = evt->get<CoreGraphicsDraw>();
            DrawLabel(event->context, event->bounds);
        } else if (rval && evt->validType<MacEventCocoa>()) {
            FB::MacEventCocoa *event = evt->get<MacEventCocoa>();
            if (NPCocoaEventDrawRect == event->msg.type) {
                FB::Rect bounds = { 0, 0, event->msg.data.draw.height, event->msg.data.draw.width };
                DrawLabel(event->msg.data.draw.context, bounds);
            }
        } else if (rval && evt->validType<MacEventCarbon>() && rval) {
            FB::MacEventCarbon *event = evt->get<MacEventCarbon>();
            if (updateEvt == event->msg.what)
                DrawLabel(m_cgContext.context, getWindowPosition());
        }
    }
    return rval;
}

