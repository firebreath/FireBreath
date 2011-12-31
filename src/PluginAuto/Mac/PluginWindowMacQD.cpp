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

#include "ConstructDefaultPluginWindows.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/DrawingEvents.h"

#include "PluginWindowMacQD.h"

using namespace FB;

#ifndef NP_NO_QUICKDRAW

FB::PluginWindowMacQD* FB::createPluginWindowMacQD()
{
    return new PluginWindowMacQD();
}

PluginWindowMacQD::PluginWindowMacQD()
  : PluginWindowMac(), m_port(), m_window(NULL)
{
}

PluginWindowMacQD::~PluginWindowMacQD()
{
}

NPError PluginWindowMacQD::SetWindow(NPWindow* window)
{
    NP_Port *prt = (NP_Port*) window->window;
//  FBLOG_TRACE("PluginCore", "NP_Port=%p CGrafPtr=%p (%d,%d) (%d,%d){%d,%d}", prt, prt->port, prt->portx, prt->porty, window->x, window->y, window->width, window->height);
    if (!prt || !IsValidPort(prt->port))
        return NPERR_INVALID_PARAM;

    if (m_port.port != prt->port)
    {
        m_port = *prt;
        m_window = GetWindowFromPort(m_port.port);

        m_x = window->x;
        m_y = window->y;
        m_width = window->width;
        m_height = window->height;
        m_clipTop = window->clipRect.top;
        m_clipLeft = window->clipRect.left;
        m_clipBottom = window->clipRect.bottom;
        m_clipRight = window->clipRect.right;

        ChangedEvent evt;
        SendEvent(&evt);
        return NPERR_NO_ERROR;
    }

    return PluginWindowMac::SetWindow(window);
}

#endif // NP_NO_QUICKDRAW
