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

#include "PluginEvents/KeyCodes.h"

namespace FB {
    FBKeyCode CarbonKeyCodeToFBKeyCode(unsigned long carbonKey);

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

