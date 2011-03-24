/**********************************************************\ 
 
 Auto-generated FactoryMain.cpp
 
 This file contains the auto-generated factory methods 
 for the BasicMediaPlayer project
 
 \**********************************************************/

#include "FactoryBase.h"
#ifdef FB_WIN32
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
#ifdef FB_WIN32
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

