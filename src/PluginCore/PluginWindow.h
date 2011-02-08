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
        /// @fn virtual void InvalidateWindow() const = 0
        ///
        /// @brief  Invalidate window.  This should tell the OS to send a REFRESH event.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void InvalidateWindow() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::Rect getWindowPosition() const
        ///
        /// @brief  Gets the position rect of the window
        ///
        /// @return The position rect of the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::Rect getWindowPosition() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual long getWindowWidth() const
        ///
        /// @brief  Gets the width of the window
        ///
        /// @return The width of the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual uint32_t getWindowWidth() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual long getWindowHeight() const
        ///
        /// @brief  Gets the height of the window
        ///
        /// @return The height of the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual uint32_t getWindowHeight() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FP::Rect getWindowClipping() const
        ///
        /// @brief  Gets the clipping rect of the window
        ///
        /// @return The clipping rect of the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::Rect getWindowClipping() const = 0;
    };
};

#endif

