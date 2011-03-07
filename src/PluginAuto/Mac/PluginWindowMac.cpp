/**********************************************************\
Original Author: Eric Herrmann 

Created:    Feb 8, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Eric Herrmann, Firebreath development team
\**********************************************************/

#include "global/config.h"
#include "FactoryBase.h"

#include "PluginWindowMacCA.h"
#include "PluginWindowMacCG.h"
#include "PluginWindowMacQD.h"
#include "PluginEventMacCocoa.h"
#include "PluginEventMacCarbon.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "Mac/NpapiPluginMac.h"
#include "NpapiPluginModule.h"
#include "PluginEventMac.h"

#include "PluginWindowMac.h"

using namespace FB;

static bool supports(const FB::Npapi::NpapiBrowserHostPtr &host, NPNVariable what) {
	NPBool value =  FALSE;        
	// err tells us if the browser supports model negotiation
	NPError err = host->GetValue(what, &value);
	return (NPERR_NO_ERROR == err && value) ? true : false;
}

static bool set(const FB::Npapi::NpapiBrowserHostPtr &host, NPPVariable what, void* value) {
	NPError err = host->SetValue(what, value);
	return (NPERR_NO_ERROR == err) ? true : false;
}

NPDrawingModel PluginWindowMac::initPluginWindowMac(const FB::Npapi::NpapiBrowserHostPtr &host) {
	NPDrawingModel drawingModel = (NPDrawingModel) -1; 
#if FBMAC_USE_COREANIMATION_INVALIDATING
	if (supports(host, NPNVsupportsInvalidatingCoreAnimationBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelInvalidatingCoreAnimation)) {
		FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelInvalidatingCoreAnimation");
		drawingModel = NPDrawingModelInvalidatingCoreAnimation;
	} else
#endif
#if FBMAC_USE_COREANIMATION
	if (supports(host, NPNVsupportsCoreAnimationBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation)) {
		FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelCoreAnimation");
		drawingModel = NPDrawingModelCoreAnimation;
	} else
#endif
#if FBMAC_USE_COREGRAPHICS
	if (supports(host, NPNVsupportsCoreGraphicsBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics)) {
		FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelCoreGraphics");
		drawingModel = NPDrawingModelCoreGraphics;
	} else
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
	if (supports(host, NPNVsupportsQuickDrawBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelQuickDraw)) {
		FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelQuickDraw");
		drawingModel = NPDrawingModelQuickDraw;
	} else
#endif
#endif
		FBLOG_INFO("PluginCore", "NPDrawingModel=NONE");
	return drawingModel;
}

FB::PluginWindowMac* PluginWindowMac::createPluginWindowMac(NPDrawingModel drawingModel) {
	FB::PluginWindowMac* rval = NULL;
	switch (drawingModel) {
#if FBMAC_USE_COREANIMATION_INVALIDATING
		case NPDrawingModelInvalidatingCoreAnimation:
			rval = getFactoryInstance()->createPluginWindowMacCA(true);
			break;
#endif
#if FBMAC_USE_COREANIMATION
		case NPDrawingModelCoreAnimation:
			rval = getFactoryInstance()->createPluginWindowMacCA(false);
			break;
#endif
#if FBMAC_USE_COREGRAPHICS
		case NPDrawingModelCoreGraphics:
			rval = getFactoryInstance()->createPluginWindowMacCG();
			break;
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
		case NPDrawingModelQuickDraw:
			rval = getFactoryInstance()->createPluginWindowMacQD();
			break;
#endif
#endif
		case NPDrawingModelOpenGL:
		default:
			break;
	}
	return rval;
}

PluginWindowMac::PluginWindowMac()
	: PluginWindow(), m_npHost()
	, m_x(0), m_y(0), m_width(0), m_height(0)
    , m_clipTop(0), m_clipLeft(0), m_clipBottom(0), m_clipRight(0)
{
}

NPError PluginWindowMac::SetWindow(NPWindow* window) {
    if (m_x != window->x
        || m_y != window->y
        || m_width != window->width
        || m_height != window->height) {
        m_x = window->x;
        m_y = window->y;
        m_width = window->width;
        m_height = window->height;
		
        ResizedEvent evt;
        SendEvent(&evt);
    }
    if (m_clipTop != window->clipRect.top
        || m_clipLeft != window->clipRect.left
        || m_clipBottom != window->clipRect.bottom
        || m_clipRight != window->clipRect.right) {
        m_clipTop = window->clipRect.top;
        m_clipLeft = window->clipRect.left;
        m_clipBottom = window->clipRect.bottom;
        m_clipRight = window->clipRect.right;
		
        ClipChangedEvent evt;
        SendEvent(&evt);
    }
	return NPERR_NO_ERROR;
}

FB::Rect PluginWindowMac::getWindowPosition() const {
	FB::Rect r = { m_y, m_x, m_y + m_height, m_x + m_width };
    return r;
}

FB::Rect PluginWindowMac::getWindowClipping() const {
	FB::Rect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

uint32_t PluginWindowMac::getWindowHeight() const {
    return m_height;
}

uint32_t PluginWindowMac::getWindowWidth() const {
    return m_width;
}

static void timerCallback(NPP npp, uint32_t timerID) {
	boost::shared_ptr<FB::Npapi::NpapiPluginMac> plugin = FB::ptr_cast<FB::Npapi::NpapiPluginMac>(FB::Npapi::getPlugin(npp));
    if (plugin)
        plugin->HandleTimerEvent();
}

int PluginWindowMac::scheduleTimer(int interval, bool repeat) {
	return m_npHost->ScheduleTimer(interval, repeat, timerCallback);
}

void PluginWindowMac::unscheduleTimer(int timerId) {
	m_npHost->UnscheduleTimer(timerId);
}

void PluginWindowMac::handleTimerEvent() {
    TimerEvent ev(0, NULL);
    SendEvent(&ev);
}
