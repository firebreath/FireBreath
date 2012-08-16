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

#include "BrowserPlugin.h"
#include "FactoryBase.h"
#include "PluginCore.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;

std::string BrowserPlugin::fileSystemPath;

BrowserPlugin::BrowserPlugin(const std::string& mimetype) :
    pluginMain(getFactoryInstance()->createPlugin(mimetype))
{
}

BrowserPlugin::~BrowserPlugin()
{
}

void BrowserPlugin::setFSPath(const std::string& path)
{
    fileSystemPath = path;
    pluginMain->setFSPath(path);
}

