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

#include "PluginEvents/KeyCodes.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn FBKeyCode GDKKeyCodeToFBKeyCode( unsigned int key )
    ///
    /// @brief  Converts a GDK key code to a FireBreath key code.
    ///
    /// @param  key  The GDK keycode.
    ///
    /// @return FireBreath keycode
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    FBKeyCode GDKKeyCodeToFBKeyCode( unsigned int key );

}

