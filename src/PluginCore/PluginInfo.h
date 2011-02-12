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

#pragma once
#ifndef H_FB_PLUGININFO
#define H_FB_PLUGININFO

#include <string>

namespace FB
{   
    std::string getPluginName(const std::string& mimetype);
    std::string getPluginDescription(const std::string& mimetype);
    bool pluginGuiEnabled();
}

#endif

