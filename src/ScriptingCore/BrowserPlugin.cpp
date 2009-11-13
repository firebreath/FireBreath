/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "BrowserPlugin.h"
#include "FactoryDefinitions.h"

using namespace FB;

BrowserPlugin::BrowserPlugin() :
    m_api(NULL)
{
    m_api = _getRootJSAPI();
}

BrowserPlugin::~BrowserPlugin()
{
}