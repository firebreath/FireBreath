/**********************************************************\ 
 
 Auto-generated Factory.cpp
 
 This file contains the auto-generated factory methods 
 for the FBTestPlugin project
 
\**********************************************************/

#include "FactoryBase.h"
#include "FBTestPlugin.h"
#include "FBTestMathPlugin.h"

class PluginFactory : public FB::FactoryBase
{
public:
    FB::PluginCorePtr createPlugin(std::string mimetype)
    {
        if (!mimetype.compare("application/x-fbtestmathplugin")) {
            return std::make_shared<FBTestMathPlugin>(mimetype);
        } else {
            // handles "application/x-fbtestplugin" and  "application/x-fbtestplugin-math"
            return std::make_shared<FBTestPlugin>(mimetype);
        }
    }
    
    void globalPluginInitialize()
    {
        FBTestPlugin::StaticInitialize();
    }
    
    void globalPluginDeinitialize()
    {
        FBTestPlugin::StaticDeinitialize();
    }
};

FB::FactoryBasePtr getFactoryInstance()
{
    static std::shared_ptr<PluginFactory> factory = std::make_shared<PluginFactory>();
    return factory;
}

