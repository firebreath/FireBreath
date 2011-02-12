/**********************************************************\
Original Author: Richard Bateman (taxilian)
                 Richard Prescott (rdprescott)

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
#ifndef H_FB_NPAPI_NPAPIPLUGINX11
#define H_FB_NPAPI_NPAPIPLUGINX11

#include "NpapiPlugin.h"

namespace FB {
    class PluginWindowX11;
    namespace Npapi {

    class NpapiPluginX11 : public NpapiPlugin
    {
    public:
        NpapiPluginX11(const NpapiBrowserHostPtr& host, const std::string& mimetype);
        virtual ~NpapiPluginX11(void);

    protected:
        FB::PluginWindowX11 *pluginWin;

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        virtual NPError GetValue(NPPVariable variable, void *);
    };

}; }; // FB::Npapi

#endif

