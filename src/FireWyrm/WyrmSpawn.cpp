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
#include "FireWyrm.h"
#include "PluginCore.h"
#include "PluginInfo.h"
#include "BrowserHost.h"
#include "LocalWyrmling.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "BrowserStreamRequest.h"
#include "WyrmSpawn.h"
//#include "WyrmStream.h"
#include "PluginEventSink.h"
using namespace FB::FireWyrm;
using FB::Promise;

#ifdef FB_WIN
extern std::string g_dllPath;
namespace {
    std::string getPluginPath()
    {
        return g_dllPath;
    }
}
#else
#include <dlfcn.h>
namespace {
    std::string getPluginPath()
    {
        ::Dl_info dlinfo;
        if (::dladdr((void*)&::FireWyrm_Init, &dlinfo) != 0) {
            return dlinfo.dli_fname;
        } else {
            return "";
        }
    }
}
#endif

WyrmSpawn::WyrmSpawn(WyrmBrowserHostPtr host, std::string mimetype)
    : FB::BrowserPlugin(mimetype),
      m_fwHost(host),
      m_isReady(false),
      m_mimetype(mimetype),
      m_pluginDesc(getFactoryInstance()->getPluginDescription(mimetype)),
    m_pluginName(getFactoryInstance()->getPluginName(mimetype))
{
    auto path(getPluginPath());
    FBLOG_INFO("WyrmSpawn", "Detecting plugin path: " << path);
    setFSPath(path);
    pluginMain->SetHost(host);
}

WyrmSpawn::~WyrmSpawn(void)
{
}

bool WyrmSpawn::setReady(Promise<void> onReady)
{
    bool rval = false;
    m_fwHost->createWyrmling(pluginMain->getRootJSAPI(), 0);
    if (!m_isReady) {
        // This is asyncronous! it will run when the browser host is ready
        onReady.done([this]() -> void {
            m_isReady = pluginMain->setReady();
        });
    }
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
}


