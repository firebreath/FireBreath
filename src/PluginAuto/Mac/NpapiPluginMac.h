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
#include "PluginWindowMac.h"
#include "PluginEventMac.h"
#include "NpapiTypes.h"


namespace FB { namespace Npapi {

    class NpapiPluginMac : public NpapiPlugin
    {        
    public:
        NpapiPluginMac(const NpapiBrowserHostPtr &host, const std::string& mimetype);
        virtual ~NpapiPluginMac(void);
        
        void init(NPMIMEType pluginType, int16_t argc, char* argn[], char *argv[]);

    protected:
        NPEventModel m_eventModel;
        NPDrawingModel m_drawingModel;
        FB::PluginWindowMacPtr pluginWin;
        FB::PluginEventMacPtr pluginEvt;
    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        int16_t HandleEvent(void* event);

        // This call is proxied from the TimerFunctor callback
        void HandleTimerEvent();
        
    private:
        int16_t GetValue(NPPVariable variable, void* value);
    };

}; }; // FB::Npapi

#endif

