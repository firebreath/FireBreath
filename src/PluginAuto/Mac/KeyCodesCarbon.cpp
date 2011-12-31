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

#include "KeyCodesCarbon.h"
#include <Carbon/Carbon.h>

namespace CarbonConst {
    enum {
        kVK_Return                    = 0x24,
        kVK_Tab                       = 0x30,
        kVK_Space                     = 0x31,
        kVK_Delete                    = 0x33,
        kVK_Escape                    = 0x35,
        kVK_Command                   = 0x37,
        kVK_Shift                     = 0x38,
        kVK_CapsLock                  = 0x39,
        kVK_Option                    = 0x3A,
        kVK_Control                   = 0x3B,
        kVK_RightShift                = 0x3C,
        kVK_RightOption               = 0x3D,
        kVK_RightControl              = 0x3E,
        kVK_Function                  = 0x3F,
        kVK_F17                       = 0x40,
        kVK_VolumeUp                  = 0x48,
        kVK_VolumeDown                = 0x49,
        kVK_Mute                      = 0x4A,
        kVK_F18                       = 0x4F,
        kVK_F19                       = 0x50,
        kVK_F20                       = 0x5A,
        kVK_F5                        = 0x60,
        kVK_F6                        = 0x61,
        kVK_F7                        = 0x62,
        kVK_F3                        = 0x63,
        kVK_F8                        = 0x64,
        kVK_F9                        = 0x65,
        kVK_F11                       = 0x67,
        kVK_F13                       = 0x69,
        kVK_F16                       = 0x6A,
        kVK_F14                       = 0x6B,
        kVK_F10                       = 0x6D,
        kVK_F12                       = 0x6F,
        kVK_F15                       = 0x71,
        kVK_Help                      = 0x72,
        kVK_Home                      = 0x73,
        kVK_PageUp                    = 0x74,
        kVK_ForwardDelete             = 0x75,
        kVK_F4                        = 0x76,
        kVK_End                       = 0x77,
        kVK_F2                        = 0x78,
        kVK_PageDown                  = 0x79,
        kVK_F1                        = 0x7A,
        kVK_LeftArrow                 = 0x7B,
        kVK_RightArrow                = 0x7C,
        kVK_DownArrow                 = 0x7D,
        kVK_UpArrow                   = 0x7E
    };
}

namespace FB {
    FBKeyCode CarbonKeyCodeToFBKeyCode(unsigned long carbonKey) {
        FBKeyCode retVal = FBKEY_UNKNOWN;

        switch(carbonKey) {
            case CarbonConst::kVK_Return : retVal = FBKEY_RETURN; break;
            case CarbonConst::kVK_Tab : retVal = FBKEY_TAB; break;
            case CarbonConst::kVK_Space : retVal = FBKEY_SPACE; break;
            case CarbonConst::kVK_CapsLock : retVal = FBKEY_CAPSLOCK; break;
            case CarbonConst::kVK_Control : retVal = FBKEY_CONTROL; break;
            case CarbonConst::kVK_Delete : retVal = FBKEY_DELETE; break;
            case CarbonConst::kVK_Escape : retVal = FBKEY_ESCAPE; break;
            case CarbonConst::kVK_Command : retVal = FBKEY_COMMAND; break;
            case CarbonConst::kVK_Shift : retVal = FBKEY_SHIFT; break;
            case CarbonConst::kVK_Option : retVal = FBKEY_OPTION; break;
            case CarbonConst::kVK_RightShift : retVal = FBKEY_RSHIFT; break;
            case CarbonConst::kVK_RightOption : retVal = FBKEY_ROPTION; break;
            case CarbonConst::kVK_RightControl : retVal = FBKEY_RCONTROL; break;
            case CarbonConst::kVK_Function : retVal = FBKEY_FUNCTION; break;
            case CarbonConst::kVK_F1 : retVal = FBKEY_F1; break;
            case CarbonConst::kVK_F2 : retVal = FBKEY_F2; break;
            case CarbonConst::kVK_F3 : retVal = FBKEY_F3; break;
            case CarbonConst::kVK_F4 : retVal = FBKEY_F4; break;
            case CarbonConst::kVK_F5 : retVal = FBKEY_F5; break;
            case CarbonConst::kVK_F6 : retVal = FBKEY_F6; break;
            case CarbonConst::kVK_F7 : retVal = FBKEY_F7; break;
            case CarbonConst::kVK_F8 : retVal = FBKEY_F8; break;
            case CarbonConst::kVK_F9 : retVal = FBKEY_F9; break;
            case CarbonConst::kVK_F10 : retVal = FBKEY_F10; break;
            case CarbonConst::kVK_F11 : retVal = FBKEY_F11; break;
            case CarbonConst::kVK_F12 : retVal = FBKEY_F12; break;
            case CarbonConst::kVK_F13 : retVal = FBKEY_F13; break;
            case CarbonConst::kVK_F14 : retVal = FBKEY_F14; break;
            case CarbonConst::kVK_F15 : retVal = FBKEY_F15; break;
            case CarbonConst::kVK_Help : retVal = FBKEY_HELP; break;
            case CarbonConst::kVK_Home : retVal = FBKEY_HOME; break;
            case CarbonConst::kVK_PageUp : retVal = FBKEY_PAGEUP; break;
            case CarbonConst::kVK_ForwardDelete : retVal = FBKEY_FORWARDDELETE; break;
            case CarbonConst::kVK_End : retVal = FBKEY_END; break;
            case CarbonConst::kVK_PageDown : retVal = FBKEY_PAGEDOWN; break;
            case CarbonConst::kVK_LeftArrow : retVal = FBKEY_LEFTARROW; break;
            case CarbonConst::kVK_RightArrow : retVal = FBKEY_RIGHTARROW; break;
            case CarbonConst::kVK_DownArrow : retVal = FBKEY_DOWNARROW; break;
            case CarbonConst::kVK_UpArrow : retVal = FBKEY_UPARROW; break;
        }
        return retVal;
    }
}

