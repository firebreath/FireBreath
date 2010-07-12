/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BROWSERPLUGIN
#define H_FB_BROWSERPLUGIN

#include "APITypes.h"
#include "AutoPtr.h"
#include "JSAPI.h"

namespace FB {

    class PluginCore;
    // This class represents an instance of a browser plugin
    // (an individual object tag, for example)
    class BrowserPlugin
    {
    public:
        BrowserPlugin();
        virtual ~BrowserPlugin();

        virtual void shutdown() = 0;
        virtual void setFSPath(std::string path);

    protected:
        std::string m_filesystemPath;
        FB::PluginCore *pluginMain;
    };

};
#endif
