#ifndef H_PLUGINWINDOWMAC_COCOA_CA
#define H_PLUGINWINDOWMAC_COCOA_CA

/**********************************************************\
 Original Author: Anson MacKeracher
 
 Created:    May 5, 2010
 License:    Dual license model; choose one of two:
 Eclipse Public License - Version 1.0
 http://www.eclipse.org/legal/epl-v10.html
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

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
        void InvalidateWindow();
        void setLayer(void* layer);
        void* getLayer() const;

    private:
        struct Impl;
        std::auto_ptr<Impl> m_impl;
    };
};
#endif // H_PLUGINWINDOWMAC_COCOA_CA
