/**********************************************************\ 
 
 Auto-generated FactoryMain.cpp
 
 This file contains the auto-generated factory methods 
 for the BasicMediaPlayer project
 
 \**********************************************************/

#include "FactoryBase.h"
#include "BasicMediaPlayerPlugin.h"
#include <boost/make_shared.hpp>

class PluginFactory : public FB::FactoryBase
{
public:
    FB::PluginCorePtr createPlugin(const std::string& mimetype)
    {
        return boost::make_shared<BasicMediaPlayerPlugin>();
    }
    
    void globalPluginInitialize()
    {
        BasicMediaPlayerPlugin::StaticInitialize();
    }
    
    void globalPluginDeinitialize()
    {
        BasicMediaPlayerPlugin::StaticDeinitialize();
    }
};

FB::FactoryBasePtr getFactoryInstance()
{
    static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    return factory;
}

