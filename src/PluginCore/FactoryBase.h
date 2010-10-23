/**********************************************************\ 
 Original Author: Georg Fritzsche
 
 Created:    Oct 23, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#pragma once 
#ifndef H_FB_FACTORYBASE
#define H_FB_FACTORYBASE

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "PluginWindowForwardDecl.h"

namespace FB
{
    class FactoryBase;
    class PluginCore;
    typedef boost::shared_ptr<FactoryBase> FactoryBasePtr;
    typedef boost::shared_ptr<PluginCore> PluginCorePtr;
    
    namespace Npapi { 
        class NpapiPlugin; 
        class NpapiBrowserHost;
        typedef boost::shared_ptr<NpapiPlugin> NpapiPluginPtr;
        typedef boost::shared_ptr<NpapiBrowserHost> NpapiBrowserHostPtr;
    }
}
    
/// @brief Get an instance of the factory class. This function *must* be defined in your plugin project.
FB::FactoryBasePtr getFactoryInstance();
    
namespace FB
{
    class FactoryBase : public boost::noncopyable
    {
    public:
        FactoryBase();
        virtual ~FactoryBase();
        
        /// @brief This has to be implemented by your plugin factory class.
        virtual FB::PluginCorePtr createPlugin() = 0;
        
        virtual void globalPluginInitialize();
        virtual void globalPluginDeinitialize();
        
        std::string getPluginName();
        std::string getPluginDesc();
        
        FB::Npapi::NpapiPluginPtr createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host);
        
#ifdef FB_WIN
        virtual PluginWindowWin*           createPluginWindowWin(const WindowContextWin&);
#endif
      
#ifdef FB_MACOSX
        virtual PluginWindowMacCarbonQD* createPluginWindowCarbonQD(const WindowContextQuickDraw&);
        virtual PluginWindowMacCarbonCG* createPluginWindowCarbonCG(const WindowContextCoreGraphics&);
        virtual PluginWindowMacCocoaCG*  createPluginWindowCocoaCG ();
        virtual PluginWindowMacCocoaCA*  createPluginWindowCocoaCA ();
        virtual PluginWindowMacCocoaICA* createPluginWindowCocoaICA();
#endif
        
#ifdef FB_X11
        virtual PluginWindowX11* createPluginWindowX11(const WindowContextX11&); 
#endif
    };
}

#endif
