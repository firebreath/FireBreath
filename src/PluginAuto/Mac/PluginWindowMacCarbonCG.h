/**********************************************************\
Original Author: Anson MacKeracher 

Created:    Jul 12, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_CARBON_CG
#define H_PLUGINWINDOWMAC_CARBON_CG

#include "PluginWindowMacCarbon.h"
#include "PluginEvents/MacEventCarbon.h"
#include "KeyCodesCarbon.h"
#include <ApplicationServices/ApplicationServices.h>
#include <AGL/agl.h>
#include "NpapiTypes.h"
#include <map>
#include "WindowContextMac.h"

namespace FB {
    class PluginWindowMacCarbonCG : public PluginWindowMacCarbon {
    public:
        PluginWindowMacCarbonCG(const WindowContextCoreGraphics&);
        virtual ~PluginWindowMacCarbonCG();

    public:
        NP_CGContext* getNPCGContext() { return m_npCGContext; }
        CGContextRef getCGContext() { return m_cgContext; }
        WindowRef getWindowRef() { return m_winRef; }

        void setContext(NP_CGContext* context);
        int16_t HandleEvent(EventRecord* evt);

    public:
        NP_CGContext* m_npCGContext;
        CGContextRef m_cgContext;
        WindowRef m_winRef;
    };
};

#endif // H_PLUGINWINDOWMAC_CARBON_CG

