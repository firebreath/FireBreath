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

#include "global/config.h"
#include <map>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "PluginInfo.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

namespace {
    bool nameParsed = false;
    std::map<std::string, std::string> descMap;

    void initPluginMetadata()
    {
        if (nameParsed)
            return;
        std::vector<std::string> versions;
        boost::algorithm::split(versions, FBSTRING_X11MIMEType, boost::is_any_of(";"));
        for (std::vector<std::string>::const_iterator it = versions.begin();
            it != versions.end(); ++it)
        {
            std::vector<std::string> parts;
            boost::algorithm::split(parts, *it, boost::is_any_of(":"));
            if (parts.size() > 2) {
                descMap[parts[0]] = parts[2];
            }
        }
        nameParsed = true;
    }
};

std::string FB::getPluginName(const std::string& mimetype)
{
    return FBSTRING_PluginName; // NPAPI only really allows one name per mimetype
}

std::string FB::getPluginDescription(const std::string& mimetype)
{
    initPluginMetadata();

    try {
        return descMap[mimetype];
    } catch (...) {
        return "Unknown Description";
    }
}

bool FB::pluginGuiEnabled() {
#if FB_GUI_DISABLED
    return false;
#else
    return true;
#endif
}

