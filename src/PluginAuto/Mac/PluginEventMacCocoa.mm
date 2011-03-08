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

using namespace FB; using namespace std;

PluginEventMacCocoa::PluginEventMacCocoa() {}

PluginEventMacCocoa::~PluginEventMacCocoa() {}

int16_t PluginEventMacCocoa::HandleEvent(void* event) {
    NPCocoaEvent* evt = (NPCocoaEvent*) event;
    PluginWindowMacPtr window = m_PluginWindow.lock();
    if (!window)
        return false;

    // Let the plugin handle the event if it wants
    MacEventCocoa macEv(evt);
    if(window->SendEvent(&macEv)) {
        return true;
    }

    // Otherwise process the event into FB platform agnostic events
    switch(evt->type) {
        case NPCocoaEventDrawRect: {
            RefreshEvent ev;
            return window->SendEvent(&ev);
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
            break;

        }

        case NPCocoaEventMouseExited: {
            break;
        }

        case NPCocoaEventMouseDragged: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseMoveEvent ev(x, y);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventKeyDown: {
            int key = (int)evt->data.key.keyCode;
            NSString* str = (NSString *)evt->data.key.characters;
            char character = [str characterAtIndex:0];
            KeyDownEvent ev(CocoaKeyCodeToFBKeyCode(key), character);
            bool rtn = window->SendEvent(&ev);
            return rtn;
            break;
        }

        case NPCocoaEventKeyUp: {
            int key = (int)evt->data.key.keyCode;
            //char character = mapCharacter(key);
            KeyUpEvent ev(CocoaKeyCodeToFBKeyCode(key), key);
            return window->SendEvent(&ev);
            break;
        }

        case NPCocoaEventFlagsChanged: {
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

        case NPCocoaEventScrollWheel: {
            break;
        }

        case NPCocoaEventTextInput: {
            // Not handled
            break;
        }
    }

    // Event was not handled by the plugin
    return false;
}
