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
#ifndef H_FB_NPAPI_NPAPIPLUGINWIN
#define H_FB_NPAPI_NPAPIPLUGINWIN

#include "win_common.h"
#include "NpapiPlugin.h"

namespace FB { 
    class PluginWindow;
    class PluginWindowWin;
    class PluginWindowlessWin;

    namespace Npapi {

    class NpapiPluginWin : public NpapiPlugin
    {
    public:
        NpapiPluginWin(const NpapiBrowserHostPtr& host, const std::string& mimetype);
        virtual ~NpapiPluginWin(void);

    protected:
        FB::PluginWindow *pluginWin;

    public:
        void invalidateWindow(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        int16_t HandleEvent(void* event);
    };

}; }; // FB::Npapi

#endif

