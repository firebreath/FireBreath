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

#include "fbjson.h"

#include <cassert>
#include "logging.h"
#include "BrowserHost.h"
#include "PluginCore.h"
#include "FactoryBase.h"
#include <assert.h>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "WyrmColony.h"
#include "WyrmBrowserHost.h"
using namespace FB::FireWyrm;

volatile uint32_t WyrmColony::ColonyInitialized(0);
WyrmColony::ColonyMap WyrmColony::m_colonyMap;
WyrmColony::CommandMap WyrmColony::cmdMap;

#define addToCommandMap(str) cmdMap[#str] = &WyrmColony:: ## str;

void WyrmColony::initCommandMap() {
    addToCommandMap(New);
    addToCommandMap(Destroy);
    addToCommandMap(Enum);
    addToCommandMap(Invoke);
    addToCommandMap(GetP);
    addToCommandMap(SetP);
    addToCommandMap(RelObj);
}

WyrmColony* WyrmColony::GetColony(FW_INST key) {
    if (!WyrmColony::ColonyInitialized) {
        FB::Log::initLogging();
        getFactoryInstance()->globalPluginInitialize();
    }
    WyrmColony* rval = NULL;
    auto module = m_colonyMap.find(key);
    if (m_colonyMap.end() == module) {
        rval = new WyrmColony(key);
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

WyrmColony::WyrmColony(FW_INST key) : m_key(key), m_threadId(std::this_thread::get_id()), m_nextSpawnId(1)
{
}

WyrmColony::~WyrmColony()
{
}

FW_RESULT FW_onCommand(const FW_INST colonyId, const uint32_t cmdId, const char* strCommand, uint32_t strCommandLen) {
    auto c = WyrmColony::GetColony(colonyId);
    std::string command(strCommand, strCommandLen);

    try {
        return c->onCommand(cmdId, command);
    } catch (std::bad_cast& e) {
        c->sendResponse(cmdId, FB::VariantList{ "error", FB::VariantMap{ { "error", "Malformed JSON request" }, { "message", std::string(e.what()) } } });
        return FW_ERR_INVALID_JSON;
    }
}

FW_RESULT FW_onCommandCallback(const FW_INST colonyId, const uint32_t cmdId, const char* strResp, uint32_t strRespLen) {
    auto c = WyrmColony::GetColony(colonyId);
    std::string response(strResp, strRespLen);

    return c->onResponse(cmdId, response);
}

void WyrmColony::setFuncs(FWHostFuncs* hFuncs) {
    memset(&m_hFuncs, 0, sizeof(*hFuncs));

    memcpy(&m_hFuncs, hFuncs, std::min(hFuncs->size, static_cast<uint16_t>(sizeof(*hFuncs))));
}

void WyrmColony::populateFuncs(FWColonyFuncs* cFuncs) {
    cFuncs->size = sizeof(*cFuncs);
    cFuncs->version = 1;

    cFuncs->call = &FW_onCommand;
    cFuncs->cmdCallback = &FW_onCommandCallback;
}

FW_RESULT WyrmColony::onCommand(const uint32_t cmdId, std::string command) {
    // This is where the magic happens... =]
    Json::Reader rdr;
    Json::Value root;

    int cmdIdx{ 0 };
    if (!rdr.parse(command, root, false)) {
        return FW_ERR_INVALID_JSON;
    } else if (!root.isArray() || root.size() < 1 || !root[cmdIdx].isString()) {
        return FW_ERR_BAD_FORMAT;
    }

    std::string cmd(root[cmdIdx].asString());

    auto fnd = cmdMap.find(cmd);
    if (fnd != cmdMap.end()) {
        // This is a valid command!
        CommandHandler cmd = fnd->second;

        VariantList args;
        for (decltype(root.size()) i = 1; i < root.size(); ++i) {
            args.emplace_back(jsonValueToVariant(root[i]));
        }
        auto dfd = (this->*cmd)(args);
        dfd.done([cmdId, this](FB::VariantList doc) {
            sendResponse(cmdId, doc);
        }, [cmdId, this](std::exception e) {
            sendResponse(cmdId, VariantList{ "error", VariantMap{ { "error", "Command threw an exception" }, { "message", e.what() } } });
        });
    } else {
        return FW_ERR_INVALID_COMMAND;
    }

    return FW_ERR_UNKNOWN;
}

FW_RESULT WyrmColony::onResponse(const uint32_t cmdId, std::string response) {
    throw std::logic_error("The method or operation is not implemented.");
}

void WyrmColony::sendResponse(const uint32_t cmdId, FB::VariantList resp) {
    auto outJSON = variantToJsonValue(resp);

    std::ostringstream out;
    out << outJSON;
    auto outDoc = out.str();
    this->m_hFuncs.cmdCallback(m_key, cmdId, outDoc.c_str(), outDoc.size());
}

FB::VariantListPromise WyrmColony::New(FB::VariantList args) {
    // Spawn a new instance
    FW_INST id = m_nextSpawnId++;
    WyrmBrowserHostPtr wyrmHost{ std::make_shared<WyrmBrowserHost>(this, id) };

    return FB::VariantList{ "success", id };
}

bool WyrmColony::_scheduleAsyncCall(void(*func)(void*), void * userData) {
    return m_hFuncs.doAsyncCall(func, userData) == 0;
}

