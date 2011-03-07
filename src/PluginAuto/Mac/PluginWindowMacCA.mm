/**********************************************************\
Original Author: Anson MacKeracher
 
Created:    May 5, 2010
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

#include "PluginWindowMacCA.h"

using namespace FB;

FB::PluginWindowMacCA* FB::createPluginWindowMacCA(bool invalidating)
{
    return new PluginWindowMacCA(invalidating);
}

PluginWindowMacCA::PluginWindowMacCA(bool invalidating)
	: PluginWindowMac(), m_layer(NULL), m_invalidating(invalidating)
{
}

PluginWindowMacCA::~PluginWindowMacCA()
{
	[(CALayer*)m_layer autorelease];
	m_layer = NULL;
}

NPError PluginWindowMacCA::SetWindow(NPWindow* window)
{
	CALayer *layer = (CALayer*) window->window;
//	FBLOG_TRACE("PluginCore", "PluginWindowMacCA::SetWindow() CALayer=%p", layer);
	if (!layer)
		return NPERR_INVALID_PARAM;

	if (m_layer != layer)
	{
		[(CALayer*)layer retain];
		[(CALayer*)m_layer autorelease];
		m_layer = layer;
		
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

void PluginWindowMacCA::InvalidateWindow() const
{
    // Force a drawInContext message to be passed to the window's CALayer
    [(CALayer*)m_layer setNeedsDisplay];
}