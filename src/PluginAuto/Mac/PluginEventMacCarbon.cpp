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
    : PluginEventMac(), m_old_h(0.0f), m_old_v(0.0f), m_mouseEntered(false)
{
}

PluginEventMacCarbon::~PluginEventMacCarbon()
{
}

Point PluginEventMacCarbon::GlobalToLocal(Point location) {
    Point rval = { location.v, location.h };
    PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
    if (pluginWindow) {    
        WindowRef window = pluginWindow->getWindowRef();
        // Convert the global mouse location to window structure location.
        HIPoint local = {location.h, location.v};
        HIPointConvert(&local, kHICoordSpaceScreenPixel, NULL, kHICoordSpaceWindow, window);

        // Get the location of the structure and port in the window.
        ::Rect structRect = {0}, portRect= {0};
        OSStatus status = GetWindowBounds(window, kWindowStructureRgn, &structRect);
        status = GetWindowBounds(window, kWindowGlobalPortRgn, &portRect);
        
#if 0   // This used to be necessary, but doesn't seem to be needed with Firefox 3.6.18 Carbon/CoreGraphics.
        // Convert window structure location to window port location by subtracting out the offset of the port in the structure.
        local.x -= portRect.left - structRect.left;
        local.y -= portRect.top - structRect.top;
#endif
        // Convert window port location to plugin location.
        FB::Rect bounds = pluginWindow->getWindowPosition();
        rval.h = local.x - bounds.left;
        rval.v = local.y - bounds.top;
    }
    return rval;
}

bool PluginEventMacCarbon::isMouseOver(Point localPt) {
    // Did mouse event happen inside the plugin's clip rect?
    bool rval = false;
    PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
    if (pluginWindow) {
        FB::Rect bounds = pluginWindow->getWindowPosition();
        localPt.h += bounds.left;
        localPt.v += bounds.top;
        FB::Rect clip = pluginWindow->getWindowClipping();
        rval = ((clip.left <= localPt.h) && (localPt.h < clip.right) && (clip.top <= localPt.v) && (localPt.v < clip.bottom));
    }
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
    CarbonToNPCocoaEvent(evt, macEvent.msgCocoa);
    if (pluginWindow->SendEvent(&macEvent)) {
        return true;
    }

    // Otherwise process the event into FireBreath platform-agnostic events
    switch (evt->what) {
        case mouseDown:
        {
            Point local = GlobalToLocal(evt->where);
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, local.h, local.v);                               
            return pluginWindow->SendEvent(&ev);
        }

        case mouseUp:
        {
            Point local = GlobalToLocal(evt->where);
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

#if 0 // Should use NPEventType_AdjustCursorEvent below.
        case nullEvent: // This is totally a hack
        {
            // Get mouse coordinates and fire an event to the plugin
            if ((this->m_old_h != evt->where.h) || (this->m_old_v != evt->where.v)) {
                this->m_old_h = evt->where.h;
                this->m_old_v = evt->where.v;
            
                Point local = GlobalToLocal(evt->where);
                
                // Did mouse event happen inside the plugin's clip rect?
                if(isMouseOver(local)) {
                    MouseMoveEvent mmEvt(local.h, local.v);
                    pluginWindow->SendEvent(&mmEvt);
                }
            }
            return false; 
        }
#endif

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
        
        case NPEventType_AdjustCursorEvent:
        {
            Point local = GlobalToLocal(evt->where);
            if(isMouseOver(local)) {
                MouseMoveEvent mmEvt(local.h, local.v);
                pluginWindow->SendEvent(&mmEvt);
            }
        }
        
        default:
            break;
    }
    return false;
}

// Include From AppKit/NSEvent.h or we'll have to convert this file to .mm
/* Device-independent bits found in event modifier flags */
enum {
    NSAlphaShiftKeyMask         = 1 << 16,
    NSShiftKeyMask              = 1 << 17,
    NSControlKeyMask            = 1 << 18,
    NSAlternateKeyMask          = 1 << 19,
    NSCommandKeyMask            = 1 << 20,
    NSNumericPadKeyMask         = 1 << 21,
    NSHelpKeyMask               = 1 << 22,
    NSFunctionKeyMask           = 1 << 23,
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    NSDeviceIndependentModifierFlagsMask    = 0xffff0000UL
#endif
};

static uint32_t modifiersForEvent(const EventRecord *event)
{
    uint32_t rval = 0;
    if (event->modifiers & cmdKey)
        rval |= NSCommandKeyMask;
    if (event->modifiers & shiftKey)
        rval |= NSShiftKeyMask;
    if (event->modifiers & alphaLock)
        rval |= NSAlphaShiftKeyMask;
    if (event->modifiers & optionKey)
        rval |= NSAlternateKeyMask;
    if (event->modifiers & controlKey)
        rval |= NSControlKeyMask;
    return rval;
}

/*
typedef enum {
√ NPCocoaEventDrawRect = 1,                // updateEvt
√ NPCocoaEventMouseDown,                   // mouseDown
√ NPCocoaEventMouseUp,                     // mouseUp
√ NPCocoaEventMouseMoved,                  // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseEntered,                // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseExited,                 // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseDragged,                // NPEventType_AdjustCursorEvent
  NPCocoaEventKeyDown,                     // NPCocoaEventKeyDown
  NPCocoaEventKeyUp,                       // NPCocoaEventKeyUp
√ NPCocoaEventFlagsChanged,                // nullEvent
√ NPCocoaEventFocusChanged,                // NPEventType_GetFocusEvent/NPEventType_LoseFocusEvent
√ NPCocoaEventWindowFocusChanged,          // activateEvt
  NPCocoaEventScrollWheel,                 // Not Supported
  NPCocoaEventTextInput                    // Not Supported
} NPCocoaEventType;

enum NPEventType {
√ nullEvent                     = 0,       // NPCocoaEventFlagsChanged
√ mouseDown                     = 1,       // NPCocoaEventMouseDown
√ mouseUp                       = 2,       // NPCocoaEventMouseUp
  keyDown                       = 3,       // NPCocoaEventKeyDown
  keyUp                         = 4,       // NPCocoaEventKeyUp
√ autoKey                       = 5,       // NPCocoaEventKeyUp
√ updateEvt                     = 6,       // NPCocoaEventDrawRect
  diskEvt                       = 7,       // Not Supported
√ activateEvt                   = 8,       // NPCocoaEventWindowFocusChanged
  osEvt                         = 15,      // Not Supported
  kHighLevelEvent               = 23       // Not Supported
√ NPEventType_GetFocusEvent = (osEvt + 16),// NPCocoaEventFocusChanged
√ NPEventType_LoseFocusEvent,              // NPCocoaEventFocusChanged
√ NPEventType_AdjustCursorEvent,           // NPCocoaEventMouseEntered/NPCocoaEventMouseExited, and NPCocoaEventMouseMoved/NPCocoaEventMouseDragged
  NPEventType_MenuCommandEvent,            // No Implementation in Mozilla or WebKit
  NPEventType_ClippingChangedEvent,        // No Implementation in Mozilla or WebKit
  NPEventType_ScrollingBeginsEvent = 1000, // No Implementation in Mozilla or WebKit
  NPEventType_ScrollingEndsEvent           // No Implementation in Mozilla or WebKit
};
*/

void PluginEventMacCarbon::CarbonToNPCocoaEvent(const EventRecord* evt, NPCocoaEvent& rval)
{
    memset(&rval, 0, sizeof(NPCocoaEvent));
    switch (evt->what) {
        case nullEvent:
        {
            rval.type = NPCocoaEventFlagsChanged;
            rval.data.key.modifierFlags = modifiersForEvent(evt);
            rval.data.key.characters = NULL;
            rval.data.key.charactersIgnoringModifiers = NULL;
            rval.data.key.isARepeat = false;
            rval.data.key.keyCode = 0;
        }   break;
        case mouseDown:
        {
            Point local = GlobalToLocal(evt->where);
            rval.type = NPCocoaEventMouseDown;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 1;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        case mouseUp:
        {
            Point local = GlobalToLocal(evt->where);
            rval.type = NPCocoaEventMouseUp;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 1;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        case keyDown:
        {
            rval.type = NPCocoaEventKeyDown;
            rval.data.key.modifierFlags = modifiersForEvent(evt);
            rval.data.key.characters = NULL;
            rval.data.key.charactersIgnoringModifiers = NULL;
            rval.data.key.isARepeat = false;
            rval.data.key.keyCode = 0;
        }   break;
        case keyUp:
        case autoKey:
        {
            rval.type = NPCocoaEventKeyUp;
            rval.data.key.modifierFlags = modifiersForEvent(evt);
            rval.data.key.characters = NULL;
            rval.data.key.charactersIgnoringModifiers = NULL;
            rval.data.key.isARepeat = false;
            rval.data.key.keyCode = 0;
        }   break;
        case updateEvt:
        {
            PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
            if (pluginWindow && PluginWindowMac::DrawingModelCoreGraphics == pluginWindow->getDrawingModel()) {
                FB::Rect bounds = pluginWindow->getWindowPosition();
                rval.type = NPCocoaEventDrawRect;
                rval.data.draw.context = (CGContextRef)pluginWindow->getDrawingPrimitive();
                rval.data.draw.x = bounds.left;
                rval.data.draw.y = bounds.top;
                rval.data.draw.width = bounds.right - bounds.left;
                rval.data.draw.height = bounds.bottom -  bounds.top;
            }
        }   break;
        case activateEvt:
        {
            rval.type = NPCocoaEventWindowFocusChanged;
            rval.data.focus.hasFocus = (evt->modifiers & activeFlag) ? true : false;
        }   break;
        case NPEventType_GetFocusEvent:
        {
            rval.type = NPCocoaEventFocusChanged;
            rval.data.focus.hasFocus = true;
        }   break;
        case NPEventType_LoseFocusEvent:
        {
            rval.type = NPCocoaEventFocusChanged;
            rval.data.focus.hasFocus = false;
        }   break;
        case NPEventType_AdjustCursorEvent:
        {
            Point local = GlobalToLocal(evt->where);
            bool mouseOver = isMouseOver(local);
            if (mouseOver && !m_mouseEntered) {
                rval.type = NPCocoaEventMouseEntered;
                m_mouseEntered = true;
            } else if (!mouseOver && m_mouseEntered) {
                rval.type = NPCocoaEventMouseExited;
                m_mouseEntered = false;
            } else
                rval.type = (evt->modifiers & btnState) ? NPCocoaEventMouseDragged : NPCocoaEventMouseMoved;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 0;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        default: {
        }   break;
    }
}

#endif
