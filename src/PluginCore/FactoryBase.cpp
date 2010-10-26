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

#include "FactoryBase.h"
#include "ConstructDefaultPluginWindows.h"
#include "NpapiPluginFactory.h"
#include "PluginInfo.h"

FB::FactoryBase::FactoryBase()
{

}

FB::FactoryBase::~FactoryBase()
{

}

void FB::FactoryBase::globalPluginInitialize()
{
    
}

void FB::FactoryBase::globalPluginDeinitialize()
{
    
}

std::string FB::FactoryBase::getPluginName()
{
    return FB::getPluginName();
}

std::string FB::FactoryBase::getPluginDescription()
{
    return FB::getPluginDescription();
}

FB::Npapi::NpapiPluginPtr FB::FactoryBase::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host)
{
    return FB::Npapi::createNpapiPlugin(host);
}

#ifdef FB_WIN
FB::PluginWindowWin* FB::FactoryBase::createPluginWindowWin(const WindowContextWin& ctx)
{
    return FB::createPluginWindowWin(ctx);
}
#endif

#ifdef FB_MACOSX
FB::PluginWindowMacCarbonQD* FB::FactoryBase::createPluginWindowCarbonQD(const FB::WindowContextQuickDraw& ctx)
{
    return FB::createPluginWindowCarbonQD(ctx);
}

FB::PluginWindowMacCarbonCG* FB::FactoryBase::createPluginWindowCarbonCG(const FB::WindowContextCoreGraphics& ctx)
{
    return FB::createPluginWindowCarbonCG(ctx);
}

FB::PluginWindowMacCocoaCG* FB::FactoryBase::createPluginWindowCocoaCG()
{
    return FB::createPluginWindowCocoaCG();
}

FB::PluginWindowMacCocoaCA* FB::FactoryBase::createPluginWindowCocoaCA()
{
    return FB::createPluginWindowCocoaCA();
}

FB::PluginWindowMacCocoaICA* FB::FactoryBase::createPluginWindowCocoaICA()
{
    return FB::createPluginWindowCocoaICA();
}
#endif

#ifdef FB_X11
FB::PluginWindowX11* FB::FactoryBase::createPluginWindowX11(const FB::WindowContextX11& ctx)
{
    return FB::createPluginWindowX11(ctx);
}
#endif

