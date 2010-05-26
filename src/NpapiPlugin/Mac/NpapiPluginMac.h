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

#ifndef H_FB_NPAPI_NPAPIPLUGINMAC
#define H_FB_NPAPI_NPAPIPLUGINMAC

#include "NpapiPlugin.h"

namespace FB {
    class PluginWindowMacQuickDraw;
    namespace Npapi {

    class NpapiPluginMac : public NpapiPlugin
    {
    public:
        NpapiPluginMac(NpapiBrowserHost *host);
        virtual ~NpapiPluginMac(void);

    protected:
        FB::PluginWindowMacQuickDraw *pluginWin;

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        int16_t HandleEvent(void* event);
    };

}; }; // FB::Npapi

#endif
