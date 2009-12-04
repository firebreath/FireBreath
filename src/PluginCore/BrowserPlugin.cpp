/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "BrowserPlugin.h"
#include "FactoryDefinitions.h"
#include "PluginCore.h"

using namespace FB;

BrowserPlugin::BrowserPlugin() :
    pluginMain(_getMainPlugin())
{
}

BrowserPlugin::~BrowserPlugin()
{
    delete pluginMain; pluginMain = NULL;
}