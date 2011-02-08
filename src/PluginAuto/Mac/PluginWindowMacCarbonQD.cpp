/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "PluginEvents/MacEventCarbon.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginWindowMacCarbonQD.h"

#include "ConstructDefaultPluginWindows.h"


using namespace FB;

FB::PluginWindowMacCarbonQD* FB::createPluginWindowCarbonQD(const FB::WindowContextQuickDraw& ctx)
{
    return new PluginWindowMacCarbonQD(ctx);
}

PluginWindowMacCarbonQD::PluginWindowMacCarbonQD(const WindowContextQuickDraw& ctx)
  : m_port(ctx.port)
{

}

PluginWindowMacCarbonQD::~PluginWindowMacCarbonQD()
{
}

int16_t PluginWindowMacCarbonQD::HandleEvent(EventRecord* evt)
{
    // Give the plugin a change to handle the event itself if desired
    MacEventCarbon macEv(evt);
    if (SendEvent(&macEv)) {
        return true;
    }
    switch (evt->what) {
        case updateEvt:
        {
            RefreshEvent refEv;
            return SendEvent(&refEv);
            break;
        }
        default:
            break;
    }
    return false;
}

