/**********************************************************\

 Auto-generated Factory.cpp

 This file contains the auto-generated factory methods
 for the @{PLUGIN_ident} project

\**********************************************************/

#include "FactoryBase.h"
#include "@{PLUGIN_ident}.h"
#include <memory>

class PluginFactory : public FB::FactoryBase
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// @@fn FB::PluginCorePtr createPlugin(const std::string& mimetype)
    ///
    /// @@brief  Creates a plugin object matching the provided mimetype
    ///         If mimetype is empty, returns the default plugin
    ///////////////////////////////////////////////////////////////////////////////
    FB::PluginCorePtr createPlugin(std::string mimetype) override
    {
        return std::make_shared<@{PLUGIN_ident}>();
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @@see FB::FactoryBase::globalPluginInitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginInitialize()
    {
        @{PLUGIN_ident}::StaticInitialize();
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @@see FB::FactoryBase::globalPluginDeinitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginDeinitialize()
    {
        @{PLUGIN_ident}::StaticDeinitialize();
    }
};

///////////////////////////////////////////////////////////////////////////////
/// @@fn getFactoryInstance()
///
/// @@brief  Returns the factory instance for this plugin module
///////////////////////////////////////////////////////////////////////////////
FB::FactoryBasePtr getFactoryInstance()
{
    static std::shared_ptr<PluginFactory> factory = std::make_shared<PluginFactory>();
    return factory;
}

