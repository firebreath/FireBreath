/**********************************************************\
Original Author: Georg Fritzsche
 
Created:    Mar 26, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
 
Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "ConstructDefaultPluginWindows.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/DrawingEvents.h"

#include "PluginWindowMacCG.h"

using namespace FB;

FB::PluginWindowMacCG* FB::createPluginWindowMacCG()
{
    return new PluginWindowMacCG();
}

PluginWindowMacCG::PluginWindowMacCG()
	: PluginWindowMac(), m_cgContext()
{
}

PluginWindowMacCG::~PluginWindowMacCG()
{
}

NPError PluginWindowMacCG::SetWindow(NPWindow* window)
{
	NP_CGContext *context = (NP_CGContext*) window->window;
//	FBLOG_TRACE("PluginCore", "PluginWindowMacCG::SetWindow() NP_CGContext=%p", context);
	if (!context)
		return NPERR_INVALID_PARAM;

	if ((m_cgContext.context != context->context)
		|| (m_cgContext.window != context->window))
	{
		m_cgContext = *context;
		m_x = window->x;
		m_y = window->y;
		m_width = window->width;
		m_height = window->height;
		m_clipTop = window->clipRect.top;
		m_clipLeft = window->clipRect.left;
		m_clipBottom = window->clipRect.bottom;
		m_clipRight = window->clipRect.right;
		
		AttachedEvent evt2;
		SendEvent(&evt2);
		return NPERR_NO_ERROR;
	}
    return PluginWindowMac::SetWindow(window);
}

void PluginWindowMacCG::InvalidateWindow() const
{
    FB::Rect pos = getWindowPosition();
    NPRect r = { pos.top, pos.left, pos.bottom, pos.right };
    if (!m_npHost->isMainThread())
        m_npHost->ScheduleOnMainThread(m_npHost, boost::bind(&Npapi::NpapiBrowserHost::InvalidateRect2, m_npHost, r));
    else
        m_npHost->InvalidateRect(&r);
}