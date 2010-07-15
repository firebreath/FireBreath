/**********************************************************\
 Original Author: Georg Fritzsche
 
 Created:    Mar 26, 2010
 License:    Dual license model; choose one of two:
 Eclipse Public License - Version 1.0
 http://www.eclipse.org/legal/epl-v10.html
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginWindowMacCarbonCG.h"

using namespace FB;

PluginWindowMacCarbonCG::PluginWindowMacCarbonCG(NP_CGContext* context) {
    this->cgContext = context;
}

PluginWindowMacCarbonCG::~PluginWindowMacCarbonCG() {
    this->clearWindow();
}

NP_CGContext* PluginWindowMacCarbonCG::getContext() {
    if (this->cgContext != NULL) {
        return this->cgContext;
    } else {
        // TODO: error
        return 0;
    }
}

void PluginWindowMacCarbonCG::setContext(NP_CGContext* context) {
    cgContext = context;
}

// Intercepting Carbon events probably should happen in PluginWindowMacCarbon,
// but for now it's here since we need a CG windowRef to calculate mouse coords
int16_t PluginWindowMacCarbonCG::HandleEvent(EventRecord* evt) {
    // Give the plugin a chance to handle the event itself if desired
    MacEventCarbon macEvent(evt);
    switch (evt->what) {
        case mouseDown:
        {
            Rect windowRect, portRect;
            int portWidth, portHeight;
            WindowRef browserWindow = (WindowRef)cgContext->window;
            GetWindowBounds(browserWindow, kWindowStructureRgn, &windowRect);
            GetWindowBounds(browserWindow, kWindowContentRgn, &portRect);
            portWidth = portRect.right - portRect.left;
            portHeight = portRect.bottom - portRect.top;
            int winHeight = windowRect.bottom - windowRect.top;
            // Query for the current coordinates of the mouse
            HIPoint* point = new HIPoint();
            point->x = evt->where.h;
            point->y = evt->where.v;
            HIPointConvert(point, kHICoordSpaceScreenPixel, NULL, kHICoordSpaceWindow, this->cgContext->window);

            int bottom = winHeight - m_height - this->getWindowPosition().top + this->getWindowClipping().top - 80;
            MouseDownEvent ev(MouseButtonEvent::MouseButton_Left,
                              point->x - m_clipLeft + m_x, 
                              bottom + point->y - m_height);
            return SendEvent(&ev);
        }

        case mouseUp:
        {
            MouseUpEvent ev(MouseButtonEvent::MouseButton_Left,
                            evt->where.v, evt->where.h);
            return SendEvent(&ev);
        }

        case keyDown:
        {
            // I don't know why, but the key codes are shifted 8 lefts left,
            // we must shift them back to their natural positions before
            // the keymap can work properly
            FBKeyCode fb_key = CarbonKeyCodeToFBKeyCode(evt->message >> 8);
            KeyDownEvent ev(fb_key, evt->message);
            return SendEvent(&ev);
        }

        case keyUp:
        {
            // I don't know why, but the key codes are shifted 8 lefts left,
            // we must shift them back to their natural positions before
            // the keymap can work properly
            FBKeyCode fb_key = CarbonKeyCodeToFBKeyCode(evt->message >> 8);
            KeyUpEvent ev(fb_key, evt->message);
            return SendEvent(&ev);
        }

        case nullEvent:
        {
            //TODO: Figure out more efficient timing mechanism
            // Get mouse coordinates and fire an event to the plugin
            CGEventRef nullEvent = CGEventCreate(NULL);
            CGPoint point = CGEventGetLocation(nullEvent);
            if((m_x < point.x) && (point.x < m_x + m_width) 
                    && (m_y < point.y) && (point.y < m_y + m_height)
                    && (point.x != this->m_old_x)
                    && (point.y != this->m_old_y)) {
                this->m_old_x = point.x;
                this->m_old_y = point.y;
                // Mouse event happened inside the plugin's window
                MouseMoveEvent mmEvt(point.x, point.y);
                SendEvent(&mmEvt);
            }

            // The plugin is still expecting a nullEvent (for drawing)
            SendEvent(&macEvent);

            return false; 
        }
    
        case updateEvt:
        {
            RefreshEvent refEv;
            return SendEvent(&refEv);
            break;
        }
        default:
            if (SendEvent(&macEvent)) {
                return true;
            }
    }
    return false;
}
