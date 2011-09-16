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
            MouseButton_Middle
        };
		enum ModifierState {
			ModifierState_Shift = 1,
			ModifierState_Control = 2,
			ModifierState_Menu = 4
		};
        MouseButtonEvent(MouseButton btn, int x, int y, unsigned int state) : m_Btn(btn), m_x(x), m_y(y), m_state(state) { }

    public:
        MouseButton m_Btn;
        int m_x;
        int m_y;
		unsigned int m_state;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseDownEvent
    ///
    /// @brief  Fired when a mouse down event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseDownEvent : public MouseButtonEvent
    {
    public:
        MouseDownEvent(MouseButton btn, int x, int y, unsigned int state) : MouseButtonEvent(btn, x, y, state) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseDoubleClickEvent
    ///
    /// @brief  Fired when a mouse double click event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseDoubleClickEvent : public MouseButtonEvent
    {
    public:
        MouseDoubleClickEvent(MouseButton btn, int x, int y, unsigned int state) : MouseButtonEvent(btn, x, y, state) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseUpEvent
    ///
    /// @brief  Fired when a mouse up event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseUpEvent : public MouseButtonEvent
    {
    public:
        MouseUpEvent(MouseButton btn, int x, int y, unsigned int state) : MouseButtonEvent(btn, x, y, state) { }
    };

};

#endif // H_FB_PLUGINEVENTS_MOUSEEVENTS

