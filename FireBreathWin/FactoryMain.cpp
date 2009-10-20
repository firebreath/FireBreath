/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "FactoryDefinitions.h"
#include "JSAPI.h"
#include "config.h"

static const char *pluginName = FBPlugin_Name;
static const char *pluginDesc = FBPlugin_Desc;

const char *_getPluginName()
{
    return pluginName;
}

const char *_getPluginDesc()
{
    return pluginDesc;
}

FB::JSAPI *_getRootJSAPI()
{
    return new FB::JSAPI();
}