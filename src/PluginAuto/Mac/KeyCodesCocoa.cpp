/**********************************************************\
  Original Author: Anson Mackeracher

Created:    Jul 12, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher and the Firebreath development team
\**********************************************************/

#include "KeyCodesCocoa.h"

namespace FB {
    FBKeyCode CocoaKeyCodeToFBKeyCode(unsigned long cocoaKey) {
        FBKeyCode retVal = FBKEY_UNKNOWN;

        /*
        switch(cocoaKey) {
            case kVK_Return : retVal = FBKEY_RETURN; break;
            case kVK_Tab : retVal = FBKEY_TAB; break;
            case kVK_Space : retVal = FBKEY_SPACE; break;
            case kVK_CapsLock : retVal = FBKEY_CAPSLOCK; break;
            case kVK_Control : retVal = FBKEY_CONTROL; break;
            case kVK_Delete : retVal = FBKEY_DELETE; break;
            case kVK_Escape : retVal = FBKEY_ESCAPE; break;
            case kVK_Command : retVal = FBKEY_COMMAND; break;
            case kVK_Shift : retVal = FBKEY_SHIFT; break;
            case kVK_Option : retVal = FBKEY_OPTION; break;
            case kVK_RightShift : retVal = FBKEY_RSHIFT; break;
            case kVK_RightOption : retVal = FBKEY_ROPTION; break;
            case kVK_RightControl : retVal = FBKEY_RCONTROL; break;
            case kVK_Function : retVal = FBKEY_FUNCTION; break;
            case kVK_F1 : retVal = FBKEY_F1; break;
            case kVK_F2 : retVal = FBKEY_F2; break;
            case kVK_F3 : retVal = FBKEY_F3; break;
            case kVK_F4 : retVal = FBKEY_F4; break;
            case kVK_F5 : retVal = FBKEY_F5; break;
            case kVK_F6 : retVal = FBKEY_F6; break;
            case kVK_F7 : retVal = FBKEY_F7; break;
            case kVK_F8 : retVal = FBKEY_F8; break;
            case kVK_F9 : retVal = FBKEY_F9; break;
            case kVK_F10 : retVal = FBKEY_F10; break;
            case kVK_F11 : retVal = FBKEY_F11; break;
            case kVK_F12 : retVal = FBKEY_F12; break;
            case kVK_F13 : retVal = FBKEY_F13; break;
            case kVK_F14 : retVal = FBKEY_F14; break;
            case kVK_F15 : retVal = FBKEY_F15; break;
            case kVK_Help : retVal = FBKEY_HELP; break;
            case kVK_Home : retVal = FBKEY_HOME; break;
            case kVK_PageUp : retVal = FBKEY_PAGEUP; break;
            case kVK_ForwardDelete : retVal = FBKEY_FORWARDDELETE; break;
            case kVK_End : retVal = FBKEY_END; break;
            case kVK_PageDown : retVal = FBKEY_PAGEDOWN; break;
            case kVK_LeftArrow : retVal = FBKEY_LEFTARROW; break;
            case kVK_RightArrow : retVal = FBKEY_RIGHTARROW; break;
            case kVK_DownArrow : retVal = FBKEY_DOWNARROW; break;
            case kVK_UpArrow : retVal = FBKEY_UPARROW; break;
        }
        */

        return retVal;
    }
}

