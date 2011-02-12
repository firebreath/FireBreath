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

#ifndef H_PLUGINWINDOWMACQD
#define H_PLUGINWINDOWMACQD

#include "PluginWindowMac.h"
#include <map>

    class PluginWindowMacQD : public PluginWindowMac
{    
        PluginWindowMacQD();
        virtual ~PluginWindowMacQD();
		NPError SetWindow(NPWindow* window);

        DrawingModel getDrawingModel() const { return DrawingModelQuickDraw; }
		void* getDrawingPrimitive() const { return m_port.port; }
		WindowRef getWindowRef() const { return m_window; }

        void InvalidateWindow() const;

        NP_Port m_port;
		WindowRef m_window;

#endif // H_PLUGINWINDOWMACQD
    protected:
        CGrafPtr m_port;
    };
};
#endif // H_PLUGINWINDOWMAC_CARBON_QD

