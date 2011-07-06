/**********************************************************\
Original Author: Peter Szucs

Created:    July 14, 2011
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Peter Szucs and the Firebreath development team
\**********************************************************/

#include "KeyCodesX11.h"

#include <gdk/gdkkeysyms.h>

namespace FB {

    FBKeyCode GDKKeyCodeToFBKeyCode( unsigned int key )
    {
        FBKeyCode retVal = FBKEY_UNKNOWN;

        switch( key )
        {
            case GDK_KEY_KP_Add           : retVal = FBKEY_ADD;           break;
            case GDK_KEY_Alt_L            : retVal = FBKEY_ALT;           break;
            case GDK_KEY_Alt_R            : retVal = FBKEY_ALT;           break;
            case GDK_KEY_Cancel           : retVal = FBKEY_CANCEL;        break;
            case GDK_KEY_Caps_Lock        : retVal = FBKEY_CAPSLOCK;      break;
            case GDK_KEY_Clear            : retVal = FBKEY_CLEAR;         break;
            case GDK_KEY_Control_L        : retVal = FBKEY_CONTROL;       break;
            case GDK_KEY_Control_R        : retVal = FBKEY_CONTROL;       break;
            case GDK_KEY_KP_Decimal       : retVal = FBKEY_DECIMAL;       break;
            case GDK_KEY_Delete           : retVal = FBKEY_DELETE;        break;
            case GDK_KEY_KP_Divide        : retVal = FBKEY_DIVIDE;        break;
            case GDK_KEY_Escape           : retVal = FBKEY_ESCAPE;        break;
            case GDK_KEY_Execute          : retVal = FBKEY_EXECUTE;       break;
            case GDK_KEY_F1               : retVal = FBKEY_F1;            break;
            case GDK_KEY_F10              : retVal = FBKEY_F10;           break;
            case GDK_KEY_F11              : retVal = FBKEY_F11;           break;
            case GDK_KEY_F12              : retVal = FBKEY_F12;           break;
            case GDK_KEY_F13              : retVal = FBKEY_F13;           break;
            case GDK_KEY_F14              : retVal = FBKEY_F14;           break;
            case GDK_KEY_F15              : retVal = FBKEY_F15;           break;
            case GDK_KEY_F16              : retVal = FBKEY_F16;           break;
            case GDK_KEY_F17              : retVal = FBKEY_F17;           break;
            case GDK_KEY_F18              : retVal = FBKEY_F18;           break;
            case GDK_KEY_F19              : retVal = FBKEY_F19;           break;
            case GDK_KEY_F2               : retVal = FBKEY_F2;            break;
            case GDK_KEY_F20              : retVal = FBKEY_F20;           break;
            case GDK_KEY_F21              : retVal = FBKEY_F21;           break;
            case GDK_KEY_F22              : retVal = FBKEY_F22;           break;
            case GDK_KEY_F3               : retVal = FBKEY_F3;            break;
            case GDK_KEY_F4               : retVal = FBKEY_F4;            break;
            case GDK_KEY_F5               : retVal = FBKEY_F5;            break;
            case GDK_KEY_F6               : retVal = FBKEY_F6;            break;
            case GDK_KEY_F7               : retVal = FBKEY_F7;            break;
            case GDK_KEY_F8               : retVal = FBKEY_F8;            break;
            case GDK_KEY_F9               : retVal = FBKEY_F9;            break;
            case GDK_KEY_Help             : retVal = FBKEY_HELP;          break;
            case GDK_KEY_Insert           : retVal = FBKEY_INSERT;        break;
            case GDK_KEY_KP_Multiply      : retVal = FBKEY_MULTIPLY;      break;
            case GDK_KEY_Num_Lock         : retVal = FBKEY_NUMLOCK;       break;
            case GDK_KEY_KP_0             : retVal = FBKEY_NUMPAD0;       break;
            case GDK_KEY_KP_1             : retVal = FBKEY_NUMPAD1;       break;
            case GDK_KEY_KP_2             : retVal = FBKEY_NUMPAD2;       break;
            case GDK_KEY_KP_3             : retVal = FBKEY_NUMPAD3;       break;
            case GDK_KEY_KP_4             : retVal = FBKEY_NUMPAD4;       break;
            case GDK_KEY_KP_5             : retVal = FBKEY_NUMPAD5;       break;
            case GDK_KEY_KP_6             : retVal = FBKEY_NUMPAD6;       break;
            case GDK_KEY_KP_7             : retVal = FBKEY_NUMPAD7;       break;
            case GDK_KEY_KP_8             : retVal = FBKEY_NUMPAD8;       break;
            case GDK_KEY_KP_9             : retVal = FBKEY_NUMPAD9;       break;
            case GDK_KEY_Page_Down        : retVal = FBKEY_PAGEDOWN;      break;
            case GDK_KEY_Page_Up          : retVal = FBKEY_PAGEUP;        break;
            case GDK_KEY_Return           : retVal = FBKEY_RETURN;        break;
            case GDK_KEY_Scroll_Lock      : retVal = FBKEY_SCROLL ;       break;
            case GDK_KEY_Select           : retVal = FBKEY_SELECT;        break;
            case GDK_KEY_KP_Separator     : retVal = FBKEY_SEPARATOR;     break;
            case GDK_KEY_Shift_L          : retVal = FBKEY_SHIFT;         break;
            case GDK_KEY_Shift_R          : retVal = FBKEY_SHIFT;         break;
            case GDK_KEY_Sys_Req          : retVal = FBKEY_PRINTSCREEN;   break;
            case GDK_KEY_space            : retVal = FBKEY_SPACE;         break;
            case GDK_KEY_KP_Subtract      : retVal = FBKEY_SUBTRACT;      break;
            case GDK_KEY_Tab              : retVal = FBKEY_TAB;           break;
            case GDK_KEY_Up               : retVal = FBKEY_UP;            break;
            case GDK_KEY_Down             : retVal = FBKEY_DOWN;          break;
            case GDK_KEY_Left             : retVal = FBKEY_LEFT;          break;
            case GDK_KEY_Right            : retVal = FBKEY_RIGHT;         break;
            case GDK_Meta_L               : retVal = FBKEY_WINDOWS_LEFT;  break;
            case GDK_Menu                 : retVal = FBKEY_WINDOWS_MENU;  break;
            case GDK_Meta_R               : retVal = FBKEY_WINDOWS_RIGHT; break;
            case GDK_KEY_BackSpace        : retVal = FBKEY_BACKSPACE;     break;
            case GDK_KEY_End              : retVal = FBKEY_END;           break;
            case GDK_KEY_Home             : retVal = FBKEY_HOME;          break;
            case GDK_KEY_Pause            : retVal = FBKEY_PAUSE;         break;
            case GDK_KEY_Print            : retVal = FBKEY_PRINT;         break;
        }

        return retVal;
    }

}

