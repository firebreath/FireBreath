/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <stdexcept>

#include <cassert>
#include "logging.h"
#include "BrowserHost.h"
#include "PluginCore.h"
#include <assert.h>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "WyrmPluginModule.h"
using namespace FB::FireWyrm;

volatile uint32_t FireWyrmPluginModule::PluginModuleInitialized(0);
FireWyrmPluginModule::Modules FireWyrmPluginModule::m_modules;

FireWyrmPluginModule* FireWyrmPluginModule::GetModule(const void* key) {
    if (!FireWyrmPluginModule::PluginModuleInitialized) {
        FB::Log::initLogging();
        getFactoryInstance()->globalPluginInitialize();
    }
    FireWyrmPluginModule* rval = NULL;
    Modules::iterator module = m_modules.find(key);
    if (m_modules.end() == module) {
        rval = new FireWyrmPluginModule();
        m_modules[key] = rval;
        FireWyrmPluginModule::PluginModuleInitialized++;
    } else
        rval = module->second;
    return rval;
}
void FireWyrmPluginModule::ReleaseModule(const void* key) {
    Modules::iterator module = m_modules.find(key);
    if (m_modules.end() != module) {
        delete module->second;
        m_modules.erase(module);
        FireWyrmPluginModule::PluginModuleInitialized--;
    }

    if (!FireWyrmPluginModule::PluginModuleInitialized) {

        getFactoryInstance()->globalPluginDeinitialize();

        // NOTE: If this assertion fails you have some sort of memory leak; BrowserHost objects
        // are reference counted, so you have a shared_ptr to your browserhost sometime. This
        // can be a big problem because BrowserHost manages a lot of your memory and if you get
        // a new one for each instances on each page (including page reloads).
        assert(BrowserHost::getInstanceCount() == 0);
        assert(PluginCore::getActivePluginCount() == 0);
        FB::Log::stopLogging();
    }
}

FireWyrmPluginModule::FireWyrmPluginModule()
{
}

FireWyrmPluginModule::~FireWyrmPluginModule()
{
}

