/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "JSAPI.h"
#include "config.h"
#include "NpapiPlugin.h"
#include "NpapiBrowserHost.h"

static const char *pluginName = FBSTRING_PluginName;
static const char *pluginDesc = FBSTRING_FileDescription;

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

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return new FB::Npapi::NpapiPlugin(host);
}