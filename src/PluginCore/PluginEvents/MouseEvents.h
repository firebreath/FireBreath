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
#ifndef H_FB_PLUGINEVENTS_MOUSEEVENTS
#define H_FB_PLUGINEVENTS_MOUSEEVENTS

#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseMoveEvent
    ///
    /// @brief  Fired when the mouse moves
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseMoveEvent : public PluginEvent
    {
    public:
        MouseMoveEvent(int x, int y) : m_x(x), m_y(y) { }

    public:
        int m_x;
        int m_y;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseButtonEvent
    ///
    /// @brief  Fired when a mouse button event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseButtonEvent : public PluginEvent
    {
    public:
        enum MouseButton {
            MouseButton_Left,
            MouseButton_Right,
            MouseButton_Middle,
            MouseButton_None
        };
        MouseButtonEvent(MouseButton btn, int x, int y) : m_Btn(btn), m_x(x), m_y(y) { }

    public:
        MouseButton m_Btn;
        int m_x;
        int m_y;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseDownEvent
    ///
    /// @brief  Fired when a mouse down event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseDownEvent : public MouseButtonEvent
    {
    public:
        MouseDownEvent(MouseButton btn, int x, int y) : MouseButtonEvent(btn, x, y) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseUpEvent
    ///
    /// @brief  Fired when a mouse up event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseUpEvent : public MouseButtonEvent
    {
    public:
        MouseUpEvent(MouseButton btn, int x, int y) : MouseButtonEvent(btn, x, y) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseScrollEvent
    ///
    /// @brief  Fired when the user moves the scrollwheel
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseScrollEvent : public PluginEvent
    {
    public:
        MouseScrollEvent(int x, int y, double dx, double dy) : m_x(x), m_y(y), m_dx(dx), m_dy(dy) { }

    public:
        int m_x;
        int m_y;
        double m_dx;
        double m_dy;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseEnteredEvent
    ///
    /// @brief  Fired when the user moves mouse over the plugin
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseEnteredEvent : public MouseMoveEvent
    {
    public:
        MouseEnteredEvent(int x, int y) : MouseMoveEvent(x,y) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseExitedEvent
    ///
    /// @brief  Fired when the user moves mouse away from the plugin
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseExitedEvent : public MouseMoveEvent
    {
    public:
        MouseExitedEvent(int x, int y) : MouseMoveEvent(x,y) { }
    };
};

#endif // H_FB_PLUGINEVENTS_MOUSEEVENTS

