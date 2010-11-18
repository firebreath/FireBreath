/**********************************************************\
Original Author: Anson MacKeracher
 
Created:    May 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginWindowMacCocoaCG.h"
#include "ConstructDefaultPluginWindows.h"

using namespace FB;

FB::PluginWindowMacCocoaCG* FB::createPluginWindowCocoaCG()
{
    return new PluginWindowMacCocoaCG();
}

PluginWindowMacCocoaCG::PluginWindowMacCocoaCG() {}

PluginWindowMacCocoaCG::~PluginWindowMacCocoaCG() {
    this->clearWindow();
}

int16_t PluginWindowMacCocoaCG::HandleEvent(NPCocoaEvent* evt) {
    // In almost all cases forward the event to our parent class,
    // except drawing events where we must scrape the CGContextRef 
    switch(evt->type) {
        case NPCocoaEventDrawRect: {
            setContext(evt->data.draw.context);
            break;
        }

        default:
            break;
    }

    // Forward event to parent class
    return PluginWindowMacCocoa::HandleEvent(evt);
}

void PluginWindowMacCocoaCG::setContext(CGContextRef context) {
    m_context = context;
}

void PluginWindowMacCocoaCG::clearWindow() {
    // TODO
}

void PluginWindowMacCocoaCG::setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void PluginWindowMacCocoaCG::setWindowClipping(uint16_t top, uint16_t left, uint16_t bottom, uint16_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
}

NPRect PluginWindowMacCocoaCG::getWindowPosition() {
    NPRect windRect;
    windRect.left = m_x;
    windRect.top = m_y;
    windRect.right = m_x + m_width;
    windRect.bottom = m_y - m_height;
    return windRect;
}

NPRect PluginWindowMacCocoaCG::getWindowClipping() {
    NPRect clipRect;
    clipRect.left = m_clipLeft;
    clipRect.right = m_clipRight;
    clipRect.top = m_clipTop;
    clipRect.bottom = m_clipBottom;
    return clipRect;    
}
