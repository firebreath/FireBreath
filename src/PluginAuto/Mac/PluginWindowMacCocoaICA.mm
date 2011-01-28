/**********************************************************\
Original Author: Anson MacKeracher 
 
Created:    Sept 17, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
 
Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#include "PluginWindowMacCocoaICA.h"
#include "ConstructDefaultPluginWindows.h"

using namespace FB;

FB::PluginWindowMacCocoaICA* FB::createPluginWindowCocoaICA()
{
    return new PluginWindowMacCocoaICA();
}

PluginWindowMacCocoaICA::PluginWindowMacCocoaICA() {}

PluginWindowMacCocoaICA::~PluginWindowMacCocoaICA() {}
