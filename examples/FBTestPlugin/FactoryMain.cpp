/**********************************************************\ 

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods 
  for the ${PLUGIN.name} project

\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "FBTestPlugin.h"

FB::PluginCore *_getMainPlugin()
{
    return new FBTestPlugin();
}

void GlobalPluginInitialize()
{
    FBTestPlugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    FBTestPlugin::StaticDeinitialize();
}
