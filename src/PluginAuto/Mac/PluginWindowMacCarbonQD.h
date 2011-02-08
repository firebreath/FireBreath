/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_CARBON_QD
#define H_PLUGINWINDOWMAC_CARBON_QD

#include <Carbon/Carbon.h>
#include "PluginWindowMacCarbon.h"
#include "WindowContextMac.h"

#include <map>

namespace FB 
{    
    class PluginWindowMacCarbonQD : public PluginWindowMacCarbon
    {
    public:
        PluginWindowMacCarbonQD(const WindowContextQuickDraw&);
        virtual ~PluginWindowMacCarbonQD();

        int16_t HandleEvent(EventRecord* evt);
        CGrafPtr getPort() const { return m_port; }
    protected:
        CGrafPtr m_port;
    };
};
#endif // H_PLUGINWINDOWMAC_CARBON_QD

