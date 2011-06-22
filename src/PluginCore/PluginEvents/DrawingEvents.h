/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINEVENTS_DRAWINGEVENTS
#define H_FB_PLUGINEVENTS_DRAWINGEVENTS

#include "PluginEvent.h"
#include "APITypes.h"

#ifndef XP_MACOSX
#define CGContextRef void*
#define CGrafPtr void*
#else
#include "npapi.h"
#endif

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  ResizedEvent
    ///
    /// @brief  Fired when the plugin window is resized
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class ResizedEvent : public PluginEvent
    {
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  ClipChangedEvent
    ///
    /// @brief  Fired when the clipping of the plugin drawing area changes
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class ClipChangedEvent : public PluginEvent
    {
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  RefreshEvent
    ///
    /// @brief  Fired when the plugin should repaint itself (such as on windows when WM_PAINT is
    ///         received)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class RefreshEvent : public PluginEvent
    {
	public:
        RefreshEvent() { }

        RefreshEvent(FB::Rect inBounds)
            : bounds(inBounds) {}
    public:
        FB::Rect bounds;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  QuickDrawDraw
    ///
    /// @brief  Mac QuickDraw Draw event
    /// @since 1.5
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class QuickDrawDraw : public RefreshEvent
    {
    public:
        QuickDrawDraw(CGrafPtr port, const FB::Rect& bounds, const FB::Rect& clip) 
            : RefreshEvent(bounds), port(port), clip(clip) {}
    public:
        CGrafPtr port;
        FB::Rect clip;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  CoreGraphicsDraw
    ///
    /// @brief  Mac CoreGraphics Draw event
    /// @since 1.5
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class CoreGraphicsDraw : public RefreshEvent
    {
    public:
        CoreGraphicsDraw(CGContextRef context, const FB::Rect& bounds, const FB::Rect& clip) 
            : RefreshEvent(bounds), context(context), clip(clip) {}
    public:
        CGContextRef context;
        FB::Rect clip;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  FocusChangedEvent
    ///
    /// @brief  Fired when the focus changes
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class FocusChangedEvent : public PluginEvent
    {
        public:
            FocusChangedEvent(bool hasFocus) : m_hasFocus(hasFocus) { }
            virtual ~FocusChangedEvent() { }

            bool hasFocus() { return m_hasFocus; }
        protected:
            bool m_hasFocus;
    };

};

#endif // H_FB_PLUGINEVENTS_DRAWINGEVENTS

