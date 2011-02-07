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

#include "PluginWindowMacCocoa.h"
#include <Foundation/NSString.h>

using namespace FB; using namespace std;

PluginWindowMacCocoa::PluginWindowMacCocoa() {}

PluginWindowMacCocoa::~PluginWindowMacCocoa() {}

int16_t PluginWindowMacCocoa::HandleEvent(NPCocoaEvent* evt) {
    // Let the plugin handle the event if it wants
    MacEventCocoa macEv(evt);
    if(SendEvent(&macEv)) {
        return true;
    }

    // Otherwise process the event into FB platform agnostic events
    switch(evt->type) {
        case NPCocoaEventDrawRect: {
            RefreshEvent ev;
            return SendEvent(&ev);
            break;
        }

        case NPCocoaEventMouseDown: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            switch(evt->data.mouse.buttonNumber) {
                case 0: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Left, x, y);
                    return SendEvent(&ev);
                    break;
                }
                case 1: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
                    return SendEvent(&ev);
                    break;
                }
                case 2: {
                    MouseDownEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
                    return SendEvent(&ev);
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
                    return SendEvent(&ev);
                    break;
                }
                case 1: {
                    MouseUpEvent ev(MouseButtonEvent::MouseButton_Right, x, y);
                    return SendEvent(&ev);
                    break;
                }
                case 2: {
                    MouseUpEvent ev(MouseButtonEvent::MouseButton_Middle, x, y);
                    return SendEvent(&ev);
                    break;
                }
            }
        }

        case NPCocoaEventMouseMoved: {
            double x = evt->data.mouse.pluginX;
            double y = evt->data.mouse.pluginY;
            MouseMoveEvent ev(x, y);
            return SendEvent(&ev);
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
            y = m_height - y; // Reposition origin to bottom left
            MouseMoveEvent ev(x, y);
            return SendEvent(&ev);
            break;
        }

        case NPCocoaEventKeyDown: {
            int key = (int)evt->data.key.keyCode;
            NSString* str = (NSString *)evt->data.key.characters;
            char character = [str characterAtIndex:0];
            KeyDownEvent ev(CocoaKeyCodeToFBKeyCode(key), character);
            bool rtn = SendEvent(&ev);
            return rtn;
            break;
        }

        case NPCocoaEventKeyUp: {
            int key = (int)evt->data.key.keyCode;
            //char character = mapCharacter(key);
            KeyUpEvent ev(CocoaKeyCodeToFBKeyCode(key), key);
            return SendEvent(&ev);
            break;
        }

        case NPCocoaEventFlagsChanged: {
            break;
        }

        case NPCocoaEventFocusChanged: {
            FocusChangedEvent ev(evt->data.focus.hasFocus);
            return SendEvent(&ev);
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

FB::Rect PluginWindowMacCocoa::getWindowPosition() const {
	FB::Rect r = { m_y, m_x, m_y + m_height, m_x + m_width };
    return r;
}

FB::Rect PluginWindowMacCocoa::getWindowClipping() const {
	FB::Rect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

uint32_t PluginWindowMacCocoa::getWindowHeight() const {
    return m_height;
}

uint32_t PluginWindowMacCocoa::getWindowWidth() const {
    return m_width;
}

void PluginWindowMacCocoa::setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height) {
    if (m_x != x || m_y != y || m_width != width || m_height != height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        RefreshEvent ev;
        SendEvent(&ev);
    }
}

void PluginWindowMacCocoa::setWindowClipping(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

void FB::timerCallback(NPP npp, uint32_t timerID) {
    FB::Npapi::NpapiPluginPtr p = FB::Npapi::getPlugin(npp);
	boost::shared_ptr<FB::Npapi::NpapiPluginMac> plugin = FB::ptr_cast<FB::Npapi::NpapiPluginMac>(p);
    if(plugin != NULL) {
        plugin->HandleCocoaTimerEvent();
    }
}

int PluginWindowMacCocoa::scheduleTimer(int interval, bool repeat) {
    if(m_npHost != NULL) {
        return m_npHost->ScheduleTimer(interval, repeat, timerCallback);
    } else {
        return 0;
    }
}

void PluginWindowMacCocoa::unscheduleTimer(int timerId) {
    if(m_npHost != NULL) {
        m_npHost->UnscheduleTimer(timerId);
    } 
}

void PluginWindowMacCocoa::InvalidateWindow() const {
    FB::Rect pos = getWindowPosition();
    NPRect r = { pos.top, pos.left, pos.bottom, pos.right };
    if (!m_npHost->isMainThread()) {
        m_npHost->ScheduleOnMainThread(m_npHost, boost::bind(&Npapi::NpapiBrowserHost::InvalidateRect2, m_npHost, r));
    } else {
        m_npHost->InvalidateRect(&r);
    }
}

void PluginWindowMacCocoa::handleTimerEvent() {
    TimerEvent ev(0, NULL);
    SendEvent(&ev);
}
