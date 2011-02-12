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

#include "PluginEvents/KeyCodes.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn FBKeyCode WinKeyCodeToFBKeyCode( unsigned int wparam )
    ///
    /// @brief  Converts a windows key code to a FireBreath key code. 
    ///
    /// @param  wparam  The wparam. 
    ///
    /// @return FireBreath keycode
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    FBKeyCode WinKeyCodeToFBKeyCode( unsigned int wparam );

}

