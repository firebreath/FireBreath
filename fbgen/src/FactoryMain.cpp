/**********************************************************\

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods
  for the @{PLUGIN_name} project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "@{PLUGIN_ident}.h"

FB::PluginCore *_getMainPlugin()
{
    return new @{PLUGIN_ident}();
}

void GlobalPluginInitialize()
{
    @{PLUGIN_ident}::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    @{PLUGIN_ident}::StaticDeinitialize();
}
