/**********************************************************\ 

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods 
  for the {{PLUGIN_NAME}} project

\**********************************************************/

#include "windows.h"

#include "FactoryDefinitions.h"
#include "{{PLUGIN_ID}}.h"

FB::PluginCore *_getMainPlugin()
{
    return new {{PLUGIN_ID}}();
}

void GlobalPluginInitialize()
{
    {{PLUGIN_ID}}::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    {{PLUGIN_ID}}::StaticDeinitialize();
}
