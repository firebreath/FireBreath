
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
}

PluginWindowMacCocoaCA::~PluginWindowMacCocoaCA() {
    this->clearWindow();
}

void PluginWindowMacCocoaCA::clearWindow() {
    // TODO
}

void PluginWindowMacCocoaCA::setLayer(void* layer) {
    m_layer = layer;
}

void PluginWindowMacCocoaCA::InvalidateWindow() {
    // Force a drawInContext message to be passed to the window's CALayer
    [(CALayer*)m_layer setNeedsDisplay];
}
