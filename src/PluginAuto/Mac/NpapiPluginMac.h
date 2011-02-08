/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_NPAPI_NPAPIPLUGINMAC
#define H_FB_NPAPI_NPAPIPLUGINMAC

#include "NpapiPlugin.h"
#include "PluginWindow.h"
#include "NpapiTypes.h"


namespace FB {
    class PluginWindowMacCarbonQD; // Carbon & QuickDraw (really deprecated)
    class PluginWindowMacCarbonCG; // Carbon & CoreGraphics
    class PluginWindowMacCocoaCG; // Cocoa & CoreGraphics
    class PluginWindowMacCocoaCA; // Cocoa & CoreAnimation
    class PluginWindowMacCocoaICA; // Cocoa & InvalidatingCoreAnimation
    
    namespace Npapi {

    class NpapiPluginMac : public NpapiPlugin
    {        
    public:
        enum EventModel
        {
            EventModelCarbon,
            EventModelCocoa
        };
        
        enum DrawingModel
        {
            DrawingModelQuickDraw,
            DrawingModelCoreGraphics,
            DrawingModelCoreAnimation,
            DrawingModelInvalidatingCoreAnimation
        };
        
    public:
        NpapiPluginMac(const NpapiBrowserHostPtr &host, const std::string& mimetype);
        virtual ~NpapiPluginMac(void);
        
        EventModel   getEventModel  () const;
        DrawingModel getDrawingModel() const;

    protected:
        FB::PluginWindow* pluginWin;

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        int16_t HandleEvent(void* event);

        // This call is proxied from the TimerFunctor callback (only used in Cocoa Event Model)
        void HandleCocoaTimerEvent();
        
    private:
        NPError SetWindowCarbonQD(NPWindow*);
        NPError SetWindowCarbonCG(NPWindow*);
        NPError SetWindowCocoaCG(NPWindow*);
        NPError SetWindowCocoaCA(NPWindow*);
        NPError SetWindowCocoaICA(NPWindow*);
        int16_t HandleEventCarbon(void* event);
        int16_t HandleEventCocoa(void* event);
        int16_t GetValue(NPPVariable variable, void* value);
        
        EventModel m_eventModel;
        DrawingModel m_drawingModel;
    };

}; }; // FB::Npapi

#endif

