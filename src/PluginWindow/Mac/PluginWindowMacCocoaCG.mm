
/**********************************************************\
 Original Author: Anson MacKeracher
 
 Created:    May 5, 2010
 License:    Dual license model; choose one of two:
 Eclipse Public License - Version 1.0
 http://www.eclipse.org/legal/epl-v10.html
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

using namespace FB;

PluginWindowMacCocoaCG::PluginWindowMacCocoaCG(NP_CGContext* context) {
    m_context = context;
}

PluginWindowMacCocoaCG::~PluginWindowMacCocoaCG() {
    this->clearWindow();
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

int16_t PluginWindowMacCocoaCG::HandleEvent(NPCocoaEvent* event) {
    return PluginWindowMacCocoa::HandleEvent(event);
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
