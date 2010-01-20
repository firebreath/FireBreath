/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWX11
#define H_PLUGINWINDOWX11

#include "PluginWindow.h"

#include <map>

namespace FB {

    class PluginWindowLinux : public PluginWindow
    {
    public:
        PluginWindowLinux(XWindow);
        virtual ~PluginWindowLinux();

//         int16_t HandleEvent(EventRecord* evt);
        void setWindowPosition(int x, int y, int w, int h);
        void getWindowPosition(int &x, int &y, int &w, int &h);
        void setWindowClipping(int t, int l, int b, int r);
        void getWindowClipping(int &t, int &l, int &b, int &r);
        CGrafPtr getWindow() { return m_port; }
    protected:
        XWindow m_port;
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
