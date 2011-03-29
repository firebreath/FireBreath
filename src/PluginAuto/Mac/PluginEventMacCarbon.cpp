/**********************************************************\
 Original Author: Anson MacKeracher 
 
 Created:    Jul 12, 2010
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Anson MacKeracher, Firebreath development team
 \**********************************************************/

#include "ConstructDefaultPluginWindows.h"
#include "KeyCodesCarbon.h"
#include "PluginEvents/MacEventCarbon.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

#include "PluginWindowMac.h"
#include "PluginEventMacCarbon.h"

using namespace FB;

#ifndef NP_NO_CARBON

PluginEventMacCarbon::PluginEventMacCarbon()
    : PluginEventMac(), m_old_h(0.0f), m_old_v(0.0f)
{
}

PluginEventMacCarbon::~PluginEventMacCarbon()
{
}

Point PluginEventMacCarbon::GlobalToLocal(Point location) {
    PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
    // Convert the global mouse location to window structure location.
    WindowRef window = pluginWindow->getWindowRef();
    HIPoint point = {location.h, location.v};
    HIPointConvert(&point, kHICoordSpaceScreenPixel, NULL, kHICoordSpaceWindow, window);
    Point rval = { point.y, point.x };

    // Convert window structure location to window port location by subtracting out the offset of the port in the structure.
    ::Rect structRect, portRect;
    OSStatus status = GetWindowBounds(window, kWindowStructureRgn, &structRect);
    status = GetWindowBounds(window, kWindowGlobalPortRgn, &portRect);
    rval.h -= portRect.left - structRect.left;
    rval.v -= portRect.top - structRect.top;
        
    return rval;
}

int16_t PluginEventMacCarbon::HandleEvent(void* event)
{
    EventRecord* evt = (EventRecord*) event;
    PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
    if (!pluginWindow)
        return false;
    
    // Give the plugin a chance to handle the event itself if desired
    MacEventCarbon macEvent(evt);
    if (pluginWindow->SendEvent(&macEvent)) {
        if(evt->what != nullEvent) {
            // Only return on success if the event is NOT a nullEvent.
            // This is because we use nullEvents to check for mouse
            // movement (which is not an event type in Carbon).
            return true;
        }
    }

    // Otherwise process the event into FireBreath platform-agnostic events
    switch (evt->what) {
        case mouseDown:
        {
            Point local = GlobalToLocal(evt->where);
            // Convert window port location to plugin location by subtracting out the offset of the plugin in the port.
            FB::Rect bounds = pluginWindow->getWindowPosition();
            local.h -= bounds.left;
            local.v -= bounds.top;
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, local.h, local.v);                               
            return pluginWindow->SendEvent(&ev);
        }

        case mouseUp:
        {
            Point local = GlobalToLocal(evt->where);
            // Convert window port location to plugin location by subtracting out the offset of the plugin in the port.
            FB::Rect bounds = pluginWindow->getWindowPosition();
            local.h -= bounds.left;
            local.v -= bounds.top;
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left, local.h, local.v);
            return pluginWindow->SendEvent(&ev);
        }

        case keyDown:
        {
            // I don't know why, but the key codes are shifted 8 bits left,
            // we must shift them back to their natural positions before
            // the keymap can work properly
            FBKeyCode fb_key = CarbonKeyCodeToFBKeyCode((evt->message & keyCodeMask) >> 8);
            unsigned int os_key = (evt->modifiers & 0xFFFF0000) | (evt->message & (charCodeMask | keyCodeMask));
            KeyDownEvent ev(fb_key, os_key);
            return pluginWindow->SendEvent(&ev);
        }

        case keyUp:
        {
            // I don't know why, but the key codes are shifted 8 bits left,
            // we must shift them back to their natural positions before
            // the keymap can work properly
            FBKeyCode fb_key = CarbonKeyCodeToFBKeyCode((evt->message & keyCodeMask) >> 8);
            unsigned int os_key = (evt->modifiers & 0xFFFF0000) | (evt->message & (charCodeMask | keyCodeMask));
            KeyUpEvent ev(fb_key, os_key);
            return pluginWindow->SendEvent(&ev);
        }

        case nullEvent: // This is totally a hack
        {
            // Get mouse coordinates and fire an event to the plugin
            if ((this->m_old_h != evt->where.h) || (this->m_old_v != evt->where.v)) {
                this->m_old_h = evt->where.h;
                this->m_old_v = evt->where.v;
            
                Point local = GlobalToLocal(evt->where);
                
                // Did mouse event happen inside the plugin's clip rect?
                FB::Rect clip = pluginWindow->getWindowClipping();
                if((clip.left <= local.h) && (local.h < clip.right) && (clip.top <= local.v) && (local.v < clip.bottom)) {
                    // Convert window port location to plugin location by subtracting out the offset of the plugin in the port.
                    FB::Rect bounds = pluginWindow->getWindowPosition();
                    local.h -= bounds.left;
                    local.v -= bounds.top;
                    MouseMoveEvent mmEvt(local.h, local.v);
                    pluginWindow->SendEvent(&mmEvt);
                }
            }
            return false; 
        }
    
        case updateEvt:
        {
            FB::Rect bounds = pluginWindow->getWindowPosition(), clip = pluginWindow->getWindowClipping();
            switch(pluginWindow->getDrawingModel()) {
                case PluginWindowMac::DrawingModelCoreGraphics: {
                    CoreGraphicsDraw ev((CGContextRef)pluginWindow->getDrawingPrimitive(), bounds, clip);
                    return pluginWindow->SendEvent(&ev);
                } break;
                case PluginWindowMac::DrawingModelQuickDraw: {
                    QuickDrawDraw ev((CGrafPtr) pluginWindow->getDrawingPrimitive(), bounds, clip);
                    return pluginWindow->SendEvent(&ev);
                } break;
                default: {
                    RefreshEvent ev;
                    return pluginWindow->SendEvent(&ev);
                } break;
            }
            break;
        }
        default:
            if (pluginWindow->SendEvent(&macEvent)) {
                return true;
            }
    }
    return false;
}

#endif
