/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINWINDOW
#define H_FB_PLUGINWINDOW

#include <string>
#include "PluginEventSource.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginWindow
    ///
    /// @brief  PluginWindow is the base class for all PluginWindow objects.
    /// 		
    /// A PluginWindow object is platform specific and contains everything you need to perform
    /// drawing operations as well as being the source of system events such as MouseEvent or
    /// KeyEvent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginWindow : public PluginEventSource
    {
    public:
        PluginWindow() { };
        virtual ~PluginWindow() { };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginWindow::InvalidateWindow() = 0
        ///
        /// @brief  Invalidate window.  This should tell the OS to send a REFRESH event.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void InvalidateWindow() = 0;
    };
};

#endif
