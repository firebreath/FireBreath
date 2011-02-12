/**********************************************************\
Original Author: Georg Fritzsche
 
Created:    Mar 26, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_EVENTS_MACEVENTCOCOA
#define H_FB_EVENTS_MACEVENTCOCOA

#ifndef XP_MACOSX
#define NPCocoaEvent void*
#else
#include "npapi.h"
#endif

#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MacEventCocoa
    ///
    /// @brief  Cocoa mac event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MacEventCocoa : public PluginEvent
    {
    public:
        MacEventCocoa(NPCocoaEvent* src)
        {
            std::memcpy(&msg, src, sizeof(NPCocoaEvent));
        }

    public:
        NPCocoaEvent msg;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MacEventCocoaDraw
    ///
    /// @brief  Cocoa Mac Draw event
    ///
    /// @author Richard Bateman
    /// @date   10/15/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MacEventCocoaDraw : public PluginEvent
    {
    public:
        MacEventCocoaDraw(CGContextRef context, double x, double y, 
                          double width, double height) 
            : m_context(context), m_x(x), m_y(y), m_width(width),   
              m_height(height) {}
    public:
        CGContextRef m_context;
        double m_x, m_y, m_width, m_height;
    };
};

#endif

