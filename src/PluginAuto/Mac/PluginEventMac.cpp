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

#include "PluginEventMacCocoa.h"
#include "PluginEventMacCarbon.h"
#include "PluginWindowMac.h"
#include "ConstructDefaultPluginWindows.h"

#include "PluginEventMac.h"

using namespace FB;

FB::PluginEventMacCarbon* FB::createPluginEventMacCarbon()
{
#if FBMAC_USE_CARBON && !defined(NP_NO_CARBON)
    return new PluginEventMacCarbon();
#else
	assert(false);
	return NULL;
#endif
}

FB::PluginEventMacCocoa* FB::createPluginEventMacCocoa()
{
#if FBMAC_USE_COCOA
    return new PluginEventMacCocoa();
#else
	assert(false);
	return NULL;
#endif
}

/*
	Apple's Safari requires NPEventModelCarbon for NPDrawingModelCoreGraphics and NPDrawingModelQuickDraw.
	Apple's Safari requires NPEventModelCocoa for NPDrawingModelCoreAnimation and NPDrawingModelInvalidatingCoreAnimation.
*/

static bool supports(const FB::Npapi::NpapiBrowserHostPtr &host, NPNVariable what) {
	NPBool value =  FALSE;        
	// err tells us if the browser supports model negotiation
	NPError err = host->GetValue(what, &value);
	return (err != NPERR_NO_ERROR) ? false : (value == FALSE) ? false : true;
}

static bool set(const FB::Npapi::NpapiBrowserHostPtr &host, NPPVariable what, void* value) {
	NPError err = host->SetValue(what, value);
	return (err != NPERR_NO_ERROR) ? false : true;
}

NPEventModel PluginEventMac::initPluginEventMac(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel drawingModel) {
	NPEventModel eventModel = (NPEventModel) -1;
#if FBMAC_USE_COCOA
	if (
		((NPDrawingModelCoreAnimation == drawingModel) || (NPDrawingModelInvalidatingCoreAnimation == drawingModel))
		&& supports(host, NPNVsupportsCocoaBool)
		&& set(host, NPPVpluginEventModel, (void*)NPEventModelCocoa))
	{
		FBLOG_INFO("PluginCore", "NPEventModel=NPEventModelCocoa");
		eventModel = NPEventModelCocoa;
	} else
#endif
#if FBMAC_USE_CARBON
#ifndef NP_NO_CARBON
	if (
		(NPDrawingModelCoreGraphics == drawingModel)
#ifndef NP_NO_QUICKDRAW
		|| (NPDrawingModelQuickDraw == drawingModel)
#endif
	)
	{
		FBLOG_INFO("PluginCore", "NPEventModel=NPEventModelCarbon");
		eventModel = NPEventModelCarbon;
	} else
#endif
#endif
		FBLOG_INFO("PluginCore", "NPEventModel=NONE");
	return eventModel;
}

FB::PluginEventMac* PluginEventMac::createPluginEventMac(NPEventModel eventModel) {
	PluginEventMac* rval = NULL;
	switch (eventModel) {
		case NPEventModelCocoa: {
#if FBMAC_USE_COCOA
			rval = getFactoryInstance()->createPluginEventMacCocoa();
#endif
		} break;
#ifndef NP_NO_CARBON
		case NPEventModelCarbon: {
#if FBMAC_USE_CARBON
			rval = getFactoryInstance()->createPluginEventMacCarbon();
#endif
		} break;
#endif
	}
	return rval;
}