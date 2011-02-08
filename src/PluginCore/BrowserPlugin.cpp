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

using namespace FB;

BrowserPlugin::BrowserPlugin(const std::string& mimetype) :
    pluginMain(getFactoryInstance()->createPlugin(mimetype))
{
}

BrowserPlugin::~BrowserPlugin()
{
}

void BrowserPlugin::setFSPath(const std::string& path)
{
    m_filesystemPath = path;
    pluginMain->setFSPath(path);
}

