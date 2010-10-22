/**********************************************************\ 

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods 
  for the ${PLUGIN.name} project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "FBTestPlugin.h"

boost::shared_ptr<FB::PluginCore> _getMainPlugin(std::string mimetype)
{
    return boost::shared_ptr<FB::PluginCore>(new FBTestPlugin());
}

void GlobalPluginInitialize()
{
    FBTestPlugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    FBTestPlugin::StaticDeinitialize();
}
