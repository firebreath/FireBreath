/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 6, 2015
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
#include "FactoryBase.h"
#include <assert.h>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "WyrmColony.h"
using namespace FB::FireWyrm;

volatile uint32_t WyrmColony::ColonyInitialized(0);
WyrmColony::ColonyMap WyrmColony::m_colonyMap;

WyrmColony* WyrmColony::GetColony(FW_INST key) {
    if (!WyrmColony::ColonyInitialized) {
        FB::Log::initLogging();
        getFactoryInstance()->globalPluginInitialize();
    }
    WyrmColony* rval = NULL;
    auto module = m_colonyMap.find(key);
    if (m_colonyMap.end() == module) {
        rval = new WyrmColony();
        m_colonyMap[key] = rval;
        WyrmColony::ColonyInitialized++;
    } else
        rval = module->second;
    return rval;
}
FW_RESULT WyrmColony::ReleaseColony(FW_INST key) {
    auto module = m_colonyMap.find(key);
    if (m_colonyMap.end() != module) {
        delete module->second;
        m_colonyMap.erase(module);
        WyrmColony::ColonyInitialized--;
    }

    if (!WyrmColony::ColonyInitialized) {

        getFactoryInstance()->globalPluginDeinitialize();

        // NOTE: If this assertion fails you have some sort of memory leak; BrowserHost objects
        // are reference counted, so you have a shared_ptr to your browserhost sometime. This
        // can be a big problem because BrowserHost manages a lot of your memory and if you get
        // a new one for each instances on each page (including page reloads).
        assert(BrowserHost::getInstanceCount() == 0);
        assert(PluginCore::getActivePluginCount() == 0);
        FB::Log::stopLogging();
    }
    return 0;
}

WyrmColony::WyrmColony()
{
}

WyrmColony::~WyrmColony()
{
}

FW_RESULT FW_onCommand(const FW_INST colonyId, const char* strCommand, uint32_t strCommandLen) {
    auto c = WyrmColony::GetColony(colonyId);
    std::string command(strCommand, strCommandLen);

    return c->onCommand(command);
}

FW_RESULT FW_onCommandCallback(const FW_INST colonyId, const char* strResp, uint32_t strRespLen) {
    auto c = WyrmColony::GetColony(colonyId);
    std::string response(strResp, strRespLen);

    return c->onResponse(response);
}

void FB::FireWyrm::WyrmColony::setFuncs(FWHostFuncs* hFuncs) {
    memset(&m_hFuncs, 0, sizeof(*hFuncs));

    memcpy(&m_hFuncs, hFuncs, std::min(hFuncs->size, static_cast<uint16_t>(sizeof(*hFuncs))));
}

void FB::FireWyrm::WyrmColony::populateFuncs(FWColonyFuncs* cFuncs) {
    cFuncs->size = sizeof(*cFuncs);
    cFuncs->version = 1;

    cFuncs->call = &FW_onCommand;
    cFuncs->cmdCallback = &FW_onCommandCallback;
}

FW_RESULT FB::FireWyrm::WyrmColony::onCommand(std::string command) {
    throw std::logic_error("The method or operation is not implemented.");
}

FW_RESULT FB::FireWyrm::WyrmColony::onResponse(std::string response) {
    throw std::logic_error("The method or operation is not implemented.");
}

