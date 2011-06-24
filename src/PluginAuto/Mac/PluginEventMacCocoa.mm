/**********************************************************\
Original Author: Anson MacKeracher 

Created:    Mar 26, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#include <Foundation/NSString.h>

#include "ConstructDefaultPluginWindows.h"
#include "KeyCodesCocoa.h"
#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/KeyboardEvents.h"

#include "PluginWindowMac.h"
#include "PluginEventMacCocoa.h"

#include <AppKit/AppKit.h>

using namespace FB; using namespace std;

PluginEventMacCocoa::PluginEventMacCocoa() :
    m_lastModifierFlags(0)
{
}

PluginEventMacCocoa::~PluginEventMacCocoa() {}

int16_t PluginEventMacCocoa::HandleEvent(void* event) {
    NPCocoaEvent* evt = (NPCocoaEvent*) event;
    PluginWindowMacPtr window = m_PluginWindow.lock();
    if (!window)
        return false;

//  if(evt) {
//      NSLog(@"Event type: %d", evt->type);
//  }
    
    // Let the plugin handle the event if it wants
    MacEventCocoa macEv(evt);
    if(window->SendEvent(&macEv)) {
        return true;
    }
    
    // Otherwise process the event into FB platform agnostic events
    switch(evt->type) {
        case NPCocoaEventDrawRect: {
            if (window->getDrawingModel() == PluginWindowMac::DrawingModelCoreGraphics) {
                FB::Rect clipRect = { evt->data.draw.y, evt->data.draw.x,
                    evt->data.draw.y + evt->data.draw.height,
                    evt->data.draw.x + evt->data.draw.width };
                FB::Rect bounds = window->getWindowPosition();
                CoreGraphicsDraw ev(evt->data.draw.context, bounds, clipRect);
                return window->SendEvent(&ev);
            } else {
                RefreshEvent ev;
                return window->SendEvent(&ev);
            }
            break;
        }

        case NPCocoaEventMouseDown: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            switch(evt->data.mouse.buttonNumber) {
                case 0: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
                case 1: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
                case 2: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
            }
        }

        case NPCocoaEventMouseUp: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            switch(evt->data.mouse.buttonNumber) {
                case 0: {
                    MouseUpEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
                case 1: {
                    MouseUpEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
                case 2: {
                    MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
                    return window->SendEvent(&ev);
                    break;
                }
            }
        }

        case NPCocoaEventMouseMoved: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseMoveEvent ev(x, y);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventMouseEntered: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseEnteredEvent ev(x, y);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventMouseExited: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseExitedEvent ev(x, y);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventMouseDragged: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseMoveEvent ev(x, y);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventScrollWheel: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseScrollEvent ev(x, y, evt->data.mouse.deltaX, evt->data.mouse.deltaY);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventKeyDown: {
            uint16_t key = (int)evt->data.key.keyCode;
            NSString* str = (NSString *)evt->data.key.charactersIgnoringModifiers;
            unsigned int character = [str characterAtIndex:0];
            KeyDownEvent ev(CocoaKeyCodeToFBKeyCode(key), character, evt->data.key.modifierFlags);
            bool rtn = window->SendEvent(&ev);
            return rtn;
            break;
        }

        case NPCocoaEventKeyUp: {
            uint16_t key = (int)evt->data.key.keyCode;
            NSString* str = (NSString *)evt->data.key.charactersIgnoringModifiers;
            unsigned int character = [str characterAtIndex:0];
            KeyUpEvent ev(CocoaKeyCodeToFBKeyCode(key), character, evt->data.key.modifierFlags);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventFlagsChanged: {
            return ProcessModifiers(evt->data.key.modifierFlags);
            break;
        }

        case NPCocoaEventFocusChanged: {
            FocusChangedEvent ev(evt->data.focus.hasFocus);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventWindowFocusChanged: {
            // Not handled
            break;
        }

        case NPCocoaEventTextInput: {
            NSString* txt = (NSString*)evt->data.text.text;
            TextEvent ev([txt cStringUsingEncoding:[NSString defaultCStringEncoding]]);
            return window->SendEvent(&ev);
            break;
        }
    }

    // Event was not handled by the plugin
    return false;
}

int16_t PluginEventMacCocoa::ProcessModifiers(uint32_t modifierFlags) {
    PluginWindowMacPtr window = m_PluginWindow.lock();
    if (!window)
        return false;
    
    uint32_t lastModifiers = m_lastModifierFlags;
    m_lastModifierFlags = modifierFlags;

    //NSLog(@"Modifiers: %u", modifierFlags);
    
    // Shift
    if (modifierFlags & NSShiftKeyMask && !(lastModifiers & NSShiftKeyMask)) {
        NSLog(@"Shift pressed!");
        KeyDownEvent ev(FBKEY_SHIFT, NSShiftKeyMask);
        return window->SendEvent(&ev);
    }
    if (!(modifierFlags & NSShiftKeyMask) && lastModifiers & NSShiftKeyMask) {
        NSLog(@"Shift depressed!");
        KeyUpEvent ev(FBKEY_SHIFT, NSShiftKeyMask);
        return window->SendEvent(&ev);
    }
    
    // Control
    if (modifierFlags & NSControlKeyMask && !(lastModifiers & NSControlKeyMask)) {
        NSLog(@"Control pressed!");
        KeyDownEvent ev(FBKEY_CONTROL, NSControlKeyMask);
        return window->SendEvent(&ev);
    }
    if (!(modifierFlags & NSControlKeyMask) && lastModifiers & NSControlKeyMask) {
        NSLog(@"Control depressed!");
        KeyUpEvent ev(FBKEY_CONTROL, NSControlKeyMask);
        return window->SendEvent(&ev);
    }
    
    // Option
    if (modifierFlags & NSAlternateKeyMask && !(lastModifiers & NSAlternateKeyMask)) {
        NSLog(@"Option pressed!");
        KeyDownEvent ev(FBKEY_OPTION, NSAlternateKeyMask);
        return window->SendEvent(&ev);
    }
    if (!(modifierFlags & NSAlternateKeyMask) && lastModifiers & NSAlternateKeyMask) {
        NSLog(@"Option depressed!");
        KeyUpEvent ev(FBKEY_OPTION, NSAlternateKeyMask);
        return window->SendEvent(&ev);
    }

    // Command
    if (modifierFlags & NSCommandKeyMask && !(lastModifiers & NSCommandKeyMask)) {
        NSLog(@"Command pressed!");
        KeyDownEvent ev(FBKEY_COMMAND, NSCommandKeyMask);
        return window->SendEvent(&ev);
    }
    if (!(modifierFlags & NSCommandKeyMask) && lastModifiers & NSCommandKeyMask) {
        NSLog(@"Command depressed!");
        KeyUpEvent ev(FBKEY_COMMAND, NSCommandKeyMask);
        return window->SendEvent(&ev);
    }
    
    return false;
}
