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
