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
  , m_x(ctx.x)
  , m_y(ctx.y)
{

}

PluginWindowMacCarbonQD::~PluginWindowMacCarbonQD()
{
}

void PluginWindowMacCarbonQD::getWindowPosition(int &x, int &y, int &w, int &h)
{
    x = m_x;
    y = m_y;
    w = m_width;
    h = m_height;
}

void PluginWindowMacCarbonQD::setWindowPosition(int x, int y, int w, int h)
{
    if (m_x != x
        || m_y != y
        || m_width != w
        || m_height != h) {
        m_x = x;
        m_y = y;
        m_width = w;
        m_height = h;
        ResizedEvent ev;
        SendEvent(&ev);
    }
}

void PluginWindowMacCarbonQD::getWindowClipping(int &t, int &l, int &b, int &r)
{
    t = m_clipTop;
    l = m_clipLeft;
    b = m_clipBottom;
    r = m_clipRight;
}

void PluginWindowMacCarbonQD::setWindowClipping(int t, int l, int b, int r)
{
    if (m_clipTop != t
        || m_clipLeft != l
        || m_clipBottom != b
        || m_clipRight != r) {
        m_clipTop = t;
        m_clipLeft = l;
        m_clipBottom = b;
        m_clipRight = r;
        ClipChangedEvent ev;
        SendEvent(&ev);
    }
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
