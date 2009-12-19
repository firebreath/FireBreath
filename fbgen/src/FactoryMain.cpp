/**********************************************************\ 

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods 
  for the @{PLUGIN.name} project

\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "@{PLUGIN.ident}Plugin.h"

FB::PluginCore *_getMainPlugin()
{
    return new @{PLUGIN.ident}Plugin();
}

void GlobalPluginInitialize()
{
    @{PLUGIN.ident}Plugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    @{PLUGIN.ident}Plugin::StaticDeinitialize();
}
