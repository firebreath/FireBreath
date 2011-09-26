/**********************************************************\ 
 
 Auto-generated Factory.cpp
 
 This file contains the auto-generated factory methods 
 for the FBTestPlugin project
 
\**********************************************************/

#include "FactoryBase.h"
#include "FBTestPlugin.h"
#include "FBTestMathPlugin.h"
#include <boost/make_shared.hpp>

class PluginFactory : public FB::FactoryBase
{
public:
    FB::PluginCorePtr createPlugin(const std::string& mimetype)
    {
        if (!mimetype.compare("application/x-fbtestmathplugin")) {
            return boost::make_shared<FBTestMathPlugin>(mimetype);
        } else {
            // handles "application/x-fbtestplugin" and  "application/x-fbtestplugin-math"
            return boost::make_shared<FBTestPlugin>(mimetype);
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
    static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    return factory;
}

