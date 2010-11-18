/**********************************************************\ 
Original Author: Georg Fritzsche
 
Created:    Oct 23, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "config.h"
#include "PluginInfo.h"

std::string FB::getPluginName()
{
    return FBSTRING_PluginName;
}

std::string FB::getPluginDescription()
{
    return FBSTRING_FileDescription;
}
