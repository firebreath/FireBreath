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

#pragma once
#ifndef H_PLUGINWINDOWMAC
#define H_PLUGINWINDOWMAC

#include <Carbon/Carbon.h>
#include "PluginWindow.h"

#include <map>

namespace FB {

    class PluginWindowMacQuickDraw : public PluginWindow
    {
    public:
        PluginWindowMacQuickDraw(CGrafPtr port, long x, long y);
        virtual ~PluginWindowMacQuickDraw();

        int16_t HandleEvent(EventRecord* evt);
        void setWindowPosition(long x, long y, long w, long h);
        void getWindowPosition(long &x, long &y, long &w, long &h);
        void setWindowClipping(long t, long l, long b, long r);
        void getWindowClipping(long &t, long &l, long &b, long &r);
        CGrafPtr getPort() { return m_port; }
        virtual void InvalidateWindow();
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

#endif // H_PLUGINWINDOWMAC

