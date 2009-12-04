/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "JSAPISimple.h"
#include "config.h"
#include "Win/NpapiPluginWin.h"
#include "NpapiBrowserHost.h"
#include "TemplatePlugin.h"

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

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return new FB::Npapi::NpapiPluginWin(host);
}

FB::PluginCore *_getMainPlugin()
{
    return new TemplatePlugin();
}

void GlobalPluginInitialize()
{
    TemplatePlugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    TemplatePlugin::StaticDeinitialize();
}