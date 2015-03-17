/**********************************************************\
 Original Author: Richard Bateman (taxilian)

 Created:    Jan 19, 2015
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#include "FactoryBase.h"
#include "PluginCore.h"
#include "PluginInfo.h"
#include "BrowserHost.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "BrowserStreamRequest.h"
#include "WyrmSpawn.h"
//#include "WyrmStream.h"
#include "PluginEventSink.h"
using namespace FB::FireWyrm;

WyrmSpawn::WyrmSpawn(WyrmBrowserHostPtr host, std::string mimetype)
    : FB::BrowserPlugin(mimetype),
      m_isReady(false),
      m_mimetype(mimetype),
      m_pluginName(getFactoryInstance()->getPluginName(mimetype)),
      m_pluginDesc(getFactoryInstance()->getPluginDescription(mimetype))
{
    pluginMain->SetHost(host);
}

WyrmSpawn::~WyrmSpawn(void)
{
}

bool WyrmSpawn::setReady()
{
    bool rval = false;
    if (!m_isReady)
        rval = m_isReady = pluginMain->setReady();
    return rval;
}

void WyrmSpawn::shutdown(void)
{
    m_fwHost->shutdown();
    pluginMain->ClearWindow();
    pluginMain->shutdown();
    m_fwHost.reset();
}

void WyrmSpawn::init(FB::VariantMap args)
{
    pluginMain->setParams(args);
    setReady();
}


