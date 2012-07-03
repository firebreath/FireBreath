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
	/// @class  MouseEvent
	///
	/// @brief  Fired when a mouse event occurs
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class MouseEvent : public PluginEvent
	{
	public:
		enum ModifierState {
			ModifierState_None = 0,
			ModifierState_Shift = 1,
			ModifierState_Control = 2,
			ModifierState_Menu = 4
		};
		MouseEvent(int x, int y, uint32_t state = ModifierState_None) :m_x(x), m_y(y), m_state(state) { }

	public:
		int m_x;
		int m_y;
		uint32_t m_state;
	};
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseMoveEvent
    ///
    /// @brief  Fired when the mouse moves
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseMoveEvent : public MouseEvent
    {
    public:
        MouseMoveEvent(int x, int y, uint32_t state = ModifierState_None) : MouseEvent(x, y, state) { }

    public:
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseButtonEvent
    ///
    /// @brief  Fired when a mouse button event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseButtonEvent : public MouseEvent
    {
    public:
        enum MouseButton {
            MouseButton_Left,
            MouseButton_Right,
            MouseButton_Middle,
            MouseButton_None
        };
        MouseButtonEvent(MouseButton btn, int x, int y, uint32_t state = ModifierState_None) : MouseEvent(x, y, state), m_Btn(btn) { }

    public:
        MouseButton m_Btn;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseDownEvent
    ///
    /// @brief  Fired when a mouse down event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseDownEvent : public MouseButtonEvent
    {
    public:
        MouseDownEvent(MouseButton btn, int x, int y, uint32_t state = ModifierState_None) : MouseButtonEvent(btn, x, y, state) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseDoubleClickEvent
    ///
    /// @brief  Fired when a mouse double click event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseDoubleClickEvent : public MouseButtonEvent
    {
    public:
        MouseDoubleClickEvent(MouseButton btn, int x, int y, uint32_t state = ModifierState_None) : MouseButtonEvent(btn, x, y, state) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseUpEvent
    ///
    /// @brief  Fired when a mouse up event occurs
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseUpEvent : public MouseButtonEvent
    {
    public:
        MouseUpEvent(MouseButton btn, int x, int y, uint32_t state = ModifierState_None) : MouseButtonEvent(btn, x, y, state) { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  MouseScrollEvent
    ///
    /// @brief  Fired when the user moves the scrollwheel
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MouseScrollEvent : public MouseEvent
    {
    public:
        MouseScrollEvent(int x, int y, double dx, double dy, uint32_t state = ModifierState_None) : MouseEvent(x, y, state), m_dx(dx), m_dy(dy) { }

    public:
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

