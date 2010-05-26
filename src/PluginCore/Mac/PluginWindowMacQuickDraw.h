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

#ifndef H_PLUGINWINDOWMAC
#define H_PLUGINWINDOWMAC

#include <Carbon/Carbon.h>
#include "PluginWindow.h"

#include <map>

namespace FB {

    class PluginWindowMacQuickDraw : public PluginWindow
    {
    public:
        PluginWindowMacQuickDraw(CGrafPtr port, int x, int y);
        virtual ~PluginWindowMacQuickDraw();

        int16_t HandleEvent(EventRecord* evt);
        void setWindowPosition(int x, int y, int w, int h);
        void getWindowPosition(int &x, int &y, int &w, int &h);
        void setWindowClipping(int t, int l, int b, int r);
        void getWindowClipping(int &t, int &l, int &b, int &r);
        CGrafPtr getPort() { return m_port; }
        virtual void InvalidateWindow();
    protected:
        CGrafPtr m_port;
        int m_x;
        int m_y;
        int m_width;
        int m_height;

        int m_clipLeft;
        int m_clipRight;
        int m_clipTop;
        int m_clipBottom;
    };

};

#endif // H_PLUGINWINDOWMAC
