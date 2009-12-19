/**********************************************************\

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods
  for the @{PLUGIN.name} project

\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "@{PLUGIN.ident}.h"

FB::PluginCore *_getMainPlugin()
{
    return new @{PLUGIN.ident}();
}

void GlobalPluginInitialize()
{
    @{PLUGIN.ident}::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    @{PLUGIN.ident}::StaticDeinitialize();
}
