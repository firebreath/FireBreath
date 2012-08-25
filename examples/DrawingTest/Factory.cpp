/**********************************************************\

 Auto-generated Factory.cpp

 This file contains the auto-generated factory methods
 for the DrawingTest project

\**********************************************************/

#include "FactoryBase.h"
#include <boost/make_shared.hpp>

#ifdef FB_WIN
#include "Win/WinDrawingTest.h"
#endif
#ifdef FB_MACOSX
#include "Mac/MacDrawingTest.h"
#endif
#ifdef FB_X11
#include "X11/X11DrawingTest.h"
#endif

class PluginFactory : public FB::FactoryBase
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// @fn FB::PluginCorePtr createPlugin(const std::string& mimetype)
    ///
    /// @brief  Creates a plugin object matching the provided mimetype
    ///         If mimetype is empty, returns the default plugin
    ///////////////////////////////////////////////////////////////////////////////
    FB::PluginCorePtr createPlugin(const std::string& mimetype)
    {
        /*
         * As we're providing different PluginCore subclasses depending on
         * the platform, here we use conditional compilation to decide which
         * one to create and return
         */
#ifdef FB_WIN
        return boost::make_shared<WinDrawingTest>();
#endif
#ifdef FB_MACOSX
        return boost::make_shared<MacDrawingTest>();
#endif
#ifdef FB_X11
        return boost::make_shared<X11DrawingTest>();
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @see FB::FactoryBase::globalPluginInitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginInitialize()
    {
        DrawingTest::StaticInitialize();
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @see FB::FactoryBase::globalPluginDeinitialize
    ///////////////////////////////////////////////////////////////////////////////
    void globalPluginDeinitialize()
    {
        DrawingTest::StaticDeinitialize();
    }
};

///////////////////////////////////////////////////////////////////////////////
/// @fn getFactoryInstance()
///
/// @brief  Returns the factory instance for this plugin module
///////////////////////////////////////////////////////////////////////////////
FB::FactoryBasePtr getFactoryInstance()
{
    static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    return factory;
}

