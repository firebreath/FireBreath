/**********************************************************\ 
 
 Auto-generated FactoryMain.cpp
 
 This file contains the auto-generated factory methods 
 for the BasicMediaPlayer project
 
 \**********************************************************/

#include "FactoryBase.h"
#if FB_WIN
#include "BasicMediaPlayerPlugin.h"
#else
#include "Mac/BasicMediaPlayerPluginMac.h"
#endif
#include <boost/make_shared.hpp>

class PluginFactory : public FB::FactoryBase
{
public:
    FB::PluginCorePtr createPlugin(const std::string& mimetype)
    {
#if FB_WIN
        return boost::make_shared<BasicMediaPlayerPlugin>();
#else
        return boost::make_shared<BasicMediaPlayerPluginMac>();
#endif
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

