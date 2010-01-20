/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_FB_NPAPI_NPAPIPLUGINX11
#define H_FB_NPAPI_NPAPIPLUGINX11

#include "NpapiPlugin.h"

namespace FB {
    class PluginWindowX11;
    namespace Npapi {

    class NpapiPluginX11 : public NpapiPlugin
    {
    public:
        NpapiPluginX11(NpapiBrowserHost *host);
        virtual ~NpapiPluginX11(void);

    protected:
        FB::PluginWindowX11 *pluginWin;

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
    };

}; }; // FB::Npapi

#endif
