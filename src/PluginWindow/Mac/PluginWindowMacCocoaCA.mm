
/**********************************************************\
 Original Author: Anson MacKeracher
 
 Created:    May 5, 2010
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
#include "PluginWindowMacCocoaCA.h"

using namespace FB;

PluginWindowMacCocoaCA::PluginWindowMacCocoaCA() {
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
    m_layer = [CALayer new];
    [m_layer setBackgroundColor:CGColorCreateGenericRGB(0.4, 0.3, 0.1, 1.0)];
}

PluginWindowMacCocoaCA::~PluginWindowMacCocoaCA() {
    this->clearWindow();
}

void PluginWindowMacCocoaCA::clearWindow() {
    // TODO
}

void PluginWindowMacCocoaCA::setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void PluginWindowMacCocoaCA::setWindowClipping(uint16_t top, uint16_t left, uint16_t bottom, uint16_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

int16_t PluginWindowMacCocoaCA::HandleEvent(NPCocoaEvent* event) {
    // Let the plugin handle the event if it wants
    //MacEventCocoa macEv(event);
    MacEventCocoa* macEv = new MacEventCocoa(event);
    if (SendEvent(macEv)) {
        return true;
    }

    // Otherwise deal with the event here
    // TODO: Implement this
    //          - see TopHatPlugin.cpp for a hints
    return false;
}

NPRect PluginWindowMacCocoaCA::getWindowPosition() {
    NPRect windRect;
    windRect.left = m_x;
    windRect.top = m_y;
    windRect.right = m_x + m_width;
    windRect.bottom = m_y - m_height;
    return windRect;
}

NPRect PluginWindowMacCocoaCA::getWindowClipping() {
    NPRect clipRect;
    clipRect.left = m_clipLeft;
    clipRect.right = m_clipRight;
    clipRect.top = m_clipTop;
    clipRect.bottom = m_clipBottom;
    return clipRect;    
}

void PluginWindowMacCocoaCA::setLayer(void* layer) {
    m_layer = layer;
}
