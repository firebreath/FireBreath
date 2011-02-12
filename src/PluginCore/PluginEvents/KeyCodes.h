/**********************************************************\ 
Original Author: Dan Groom

Created:    April 2, 2010
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Groom and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINEVENTS_KEYCODES
#define H_FB_PLUGINEVENTS_KEYCODES

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @enum   FBKeyCode
    ///
    /// @brief  Values that represent different keys in a platform agnostic way
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    enum FBKeyCode
    {       
        FBKEY_ADD,
        FBKEY_KEYCODE_MIN = FBKEY_ADD,
        FBKEY_ALT,
        FBKEY_CANCEL,
        FBKEY_CAPSLOCK,
        FBKEY_CLEAR,
        FBKEY_CONTROL,
        FBKEY_DECIMAL,
        FBKEY_DELETE,
        FBKEY_DIVIDE,
        FBKEY_ESCAPE,
        FBKEY_EXECUTE,
        FBKEY_F1,
        FBKEY_F10,
        FBKEY_F11,
        FBKEY_F12,
        FBKEY_F13,
        FBKEY_F14,
        FBKEY_F15,
        FBKEY_F16,
        FBKEY_F17,
        FBKEY_F18,
        FBKEY_F19,
        FBKEY_F2,
        FBKEY_F20,
        FBKEY_F21,
        FBKEY_F22,
        FBKEY_F3,
        FBKEY_F4,
        FBKEY_F5,
        FBKEY_F6,
        FBKEY_F7,
        FBKEY_F8,
        FBKEY_F9,
        FBKEY_HELP,
        FBKEY_INSERT,
        FBKEY_LBUTTON,
        FBKEY_MBUTTON,
        FBKEY_MULTIPLY,
        FBKEY_NUMLOCK,
        FBKEY_NUMPAD0,
        FBKEY_NUMPAD1,
        FBKEY_NUMPAD2,
        FBKEY_NUMPAD3,
        FBKEY_NUMPAD4,
        FBKEY_NUMPAD5,
        FBKEY_NUMPAD6,
        FBKEY_NUMPAD7,
        FBKEY_NUMPAD8,
        FBKEY_NUMPAD9,
        FBKEY_PAGEDOWN,
        FBKEY_PAGEUP,
        FBKEY_RBUTTON,
        FBKEY_RETURN,
        FBKEY_SCROLL ,
        FBKEY_SELECT,
        FBKEY_SEPARATOR,
        FBKEY_SHIFT,
        FBKEY_PRINTSCREEN,
        FBKEY_SPACE,        
        FBKEY_SUBTRACT,
        FBKEY_TAB,
        FBKEY_UP,
        FBKEY_DOWN,
        FBKEY_LEFT,
        FBKEY_RIGHT,
        FBKEY_WINDOWS_LEFT,
        FBKEY_WINDOWS_MENU,
        FBKEY_WINDOWS_RIGHT,
        FBKEY_BACKSPACE,
        FBKEY_END,
        FBKEY_HOME,
        FBKEY_PAUSE,
        FBKEY_PRINT,
        FBKEY_COMMAND,
        FBKEY_OPTION,
        FBKEY_RSHIFT,
        FBKEY_ROPTION,
        FBKEY_RCONTROL,
        FBKEY_FUNCTION,
        FBKEY_FORWARDDELETE,
        FBKEY_LEFTARROW,
        FBKEY_RIGHTARROW,
        FBKEY_DOWNARROW,
        FBKEY_UPARROW,
        FBKEY_UNKNOWN,
        FBKEY_KEYCODE_MAX = FBKEY_UNKNOWN,
    };
};

#endif // H_FBKEY_PLUGINEVENTS_KEYCODES


