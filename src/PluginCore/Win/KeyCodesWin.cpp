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

#include "win_common.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include "KeyCodesWin.h"

namespace FB {

    FBKeyCode WinKeyCodeToFBKeyCode( unsigned int wparam )
    {
        FBKeyCode retVal = FBKEY_UNKNOWN;
        
        switch( wparam )
        {
            case VK_ADD         : retVal = FBKEY_ADD;           break;
            case VK_MENU        : retVal = FBKEY_ALT;           break;
            case VK_CANCEL      : retVal = FBKEY_CANCEL;        break;
            case VK_CAPITAL     : retVal = FBKEY_CAPSLOCK;      break;
            case VK_CLEAR       : retVal = FBKEY_CLEAR;         break;
            case VK_CONTROL     : retVal = FBKEY_CONTROL;       break;
            case VK_DECIMAL     : retVal = FBKEY_DECIMAL;       break;
            case VK_DELETE      : retVal = FBKEY_DELETE;        break;
            case VK_DIVIDE      : retVal = FBKEY_DIVIDE;        break;
            case VK_ESCAPE      : retVal = FBKEY_ESCAPE;        break;
            case VK_EXECUTE     : retVal = FBKEY_EXECUTE;       break;
            case VK_F1          : retVal = FBKEY_F1;            break;
            case VK_F10         : retVal = FBKEY_F10;           break;
            case VK_F11         : retVal = FBKEY_F11;           break;
            case VK_F12         : retVal = FBKEY_F12;           break;
            case VK_F13         : retVal = FBKEY_F13;           break;
            case VK_F14         : retVal = FBKEY_F14;           break;
            case VK_F15         : retVal = FBKEY_F15;           break;
            case VK_F16         : retVal = FBKEY_F16;           break;
            case VK_F17         : retVal = FBKEY_F17;           break;
            case VK_F18         : retVal = FBKEY_F18;           break;
            case VK_F19         : retVal = FBKEY_F19;           break;
            case VK_F2          : retVal = FBKEY_F2;            break;
            case VK_F20         : retVal = FBKEY_F20;           break;
            case VK_F21         : retVal = FBKEY_F21;           break;
            case VK_F22         : retVal = FBKEY_F22;           break;
            case VK_F3          : retVal = FBKEY_F3;            break;
            case VK_F4          : retVal = FBKEY_F4;            break;
            case VK_F5          : retVal = FBKEY_F5;            break;
            case VK_F6          : retVal = FBKEY_F6;            break;
            case VK_F7          : retVal = FBKEY_F7;            break;
            case VK_F8          : retVal = FBKEY_F8;            break;
            case VK_F9          : retVal = FBKEY_F9;            break;
            case VK_HELP        : retVal = FBKEY_HELP;          break;
            case VK_INSERT      : retVal = FBKEY_INSERT;        break;
            case VK_LBUTTON     : retVal = FBKEY_LBUTTON;       break;
            case VK_MBUTTON     : retVal = FBKEY_MBUTTON;       break;
            case VK_MULTIPLY    : retVal = FBKEY_MULTIPLY;      break;
            case VK_NUMLOCK     : retVal = FBKEY_NUMLOCK;       break;
            case VK_NUMPAD0     : retVal = FBKEY_NUMPAD0;       break;
            case VK_NUMPAD1     : retVal = FBKEY_NUMPAD1;       break;
            case VK_NUMPAD2     : retVal = FBKEY_NUMPAD2;       break;
            case VK_NUMPAD3     : retVal = FBKEY_NUMPAD3;       break;
            case VK_NUMPAD4     : retVal = FBKEY_NUMPAD4;       break;
            case VK_NUMPAD5     : retVal = FBKEY_NUMPAD5;       break;
            case VK_NUMPAD6     : retVal = FBKEY_NUMPAD6;       break;
            case VK_NUMPAD7     : retVal = FBKEY_NUMPAD7;       break;
            case VK_NUMPAD8     : retVal = FBKEY_NUMPAD8;       break;
            case VK_NUMPAD9     : retVal = FBKEY_NUMPAD9;       break;          
            case VK_NEXT        : retVal = FBKEY_PAGEDOWN;      break;
            case VK_PRIOR       : retVal = FBKEY_PAGEUP;        break;
            case VK_RBUTTON     : retVal = FBKEY_RBUTTON;       break;
            case VK_RETURN      : retVal = FBKEY_RETURN;        break;
            case VK_SCROLL      : retVal = FBKEY_SCROLL ;       break;
            case VK_SELECT      : retVal = FBKEY_SELECT;        break;
            case VK_SEPARATOR   : retVal = FBKEY_SEPARATOR;     break;
            case VK_SHIFT       : retVal = FBKEY_SHIFT;         break;
            case VK_SNAPSHOT    : retVal = FBKEY_PRINTSCREEN;   break;
            case VK_SPACE       : retVal = FBKEY_SPACE;         break;          
            case VK_SUBTRACT    : retVal = FBKEY_SUBTRACT;      break;
            case VK_TAB         : retVal = FBKEY_TAB;           break;
            case VK_UP          : retVal = FBKEY_UP;            break;
            case VK_DOWN        : retVal = FBKEY_DOWN;          break;
            case VK_LEFT        : retVal = FBKEY_LEFT;          break;
            case VK_RIGHT       : retVal = FBKEY_RIGHT;         break;
            case VK_LWIN        : retVal = FBKEY_WINDOWS_LEFT;  break;
            case VK_APPS        : retVal = FBKEY_WINDOWS_MENU;  break;
            case VK_RWIN        : retVal = FBKEY_WINDOWS_RIGHT; break;
            case VK_BACK        : retVal = FBKEY_BACKSPACE;     break;
            case VK_END         : retVal = FBKEY_END;           break;
            case VK_HOME        : retVal = FBKEY_HOME;          break;
            case VK_PAUSE       : retVal = FBKEY_PAUSE;         break;
            case VK_PRINT       : retVal = FBKEY_PRINT;         break;
        }

        return retVal;
    }

}

