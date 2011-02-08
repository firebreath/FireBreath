/**********************************************************\
Original Author: Anson MacKeracher 

Created:    May 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_COCOA_CA
#define H_PLUGINWINDOWMAC_COCOA_CA

#include <QuartzCore/CoreAnimation.h>

#include "Mac/PluginWindowMacCocoa.h"
#include "PluginEvents/MacEventCocoa.h"
#include "NpapiTypes.h"
#include <map>

namespace FB {
    class PluginWindowMacCocoaCA: public PluginWindowMacCocoa
    {
    public:
        PluginWindowMacCocoaCA();
        virtual ~PluginWindowMacCocoaCA();

    public:
        void clearWindow();
        void InvalidateWindow() const;
        void setLayer(void* layer);
        void* getLayer() const;

    private:
        struct Impl;
        std::auto_ptr<Impl> m_impl;
    };
};
#endif // H_PLUGINWINDOWMAC_COCOA_CA

