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

#ifndef H_PLUGINWINDOWMAC
#define H_PLUGINWINDOWMAC

#include "PluginWindow.h"

#include <map>

namespace FB {

    enum MacGraphicsModel
    {
        CoreGraphics = 0,
        QuickDraw
    };

    enum MacEventModel
    {
        Cocoa,
        Carbon
    }

    class PluginWindowMac : public PluginWindow
    {
    public:
        PluginWindowMac();
        virtual ~PluginWindowMac();

    protected:
        MacGraphicsModel m_drawingModel;
        MacEventModel m_eventModel;

    public:
        MacGraphicsModel getGraphicsModel();
        MacEventModel getEventModel();
    };

};

#endif // H_PLUGINWINDOWMAC
