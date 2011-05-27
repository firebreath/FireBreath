/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

// FBControl.cpp : Implementation of CFBControl
#include "FBControl.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

namespace {
    volatile bool g_Initialized;
}

void FB::ActiveX::flagStaticInitialized( bool init /*= true*/ )
{
    g_Initialized = init;
}

bool FB::ActiveX::isStaticInitialized()
{
    return g_Initialized;
}


