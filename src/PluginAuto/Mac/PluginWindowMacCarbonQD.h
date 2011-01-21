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
#include "PluginWindow.h"
#include "WindowContextMac.h"

#include <map>

namespace FB 
{    
    class PluginWindowMacCarbonQD : public PluginWindow
    {
    public:
        PluginWindowMacCarbonQD(const WindowContextQuickDraw&);
        virtual ~PluginWindowMacCarbonQD();

        int16_t HandleEvent(EventRecord* evt);
        void setWindowPosition(long x, long y, long w, long h);
        void getWindowPosition(long &x, long &y, long &w, long &h) const;
		long getWindowWidth() const { return m_width; }
		long getWindowHeight() const { return m_height; }
		FB::Rect getWindowPosition() const;
        void setWindowClipping(long t, long l, long b, long r);
        void getWindowClipping(long &t, long &l, long &b, long &r) const;
		FB::Rect getWindowClipping() const;
        void InvalidateWindow() const {}
        CGrafPtr getPort() const { return m_port; }
    protected:
        CGrafPtr m_port;
        long m_x;
        long m_y;
        long m_width;
        long m_height;
        
        long m_clipLeft;
        long m_clipRight;
        long m_clipTop;
        long m_clipBottom;
    };
};
#endif // H_PLUGINWINDOWMAC_CARBON_QD
