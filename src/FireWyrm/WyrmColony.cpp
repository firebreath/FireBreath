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
#include "WyrmVariantUtil.h"
#include "LocalWyrmling.h"
#include "WyrmSpawn.h"
#include "AlienWyrmling.h"
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

using namespace FB::FireWyrm;

namespace FB {
    namespace FireWyrm {
        class WyrmSac
        {
        public:
            WyrmSac(WyrmSpawnPtr spawn, WyrmBrowserHostPtr host) : spawn(spawn), host(host) {}

            WyrmSpawnPtr spawn;
            WyrmBrowserHostPtr host;
        };
        
        struct LocalMethodWyrmling {
            LocalMethodWyrmling() {}
            LocalMethodWyrmling(LocalWyrmling wl, std::string method, WyrmBrowserHostPtr host) : wyrmling(wl), method(method), host(host) {};
            LocalWyrmling wyrmling;
            std::string method;
            WyrmBrowserHostPtr host;
        };
    }
}

volatile uint32_t WyrmColony::ColonyInitialized(0);
WyrmColony::ColonyMap WyrmColony::m_colonyMap;
WyrmColony::CommandMap WyrmColony::cmdMap;

using LocalMethodMap = std::map<WyrmlingKey, LocalMethodWyrmling>;
LocalMethodMap localMethodMap;

#define addToCommandMap(str) cmdMap[BOOST_PP_STRINGIZE(str)] = &WyrmColony :: str

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
        
        localMethodMap.clear();

        // NOTE: If this assertion fails you have some sort of memory leak; BrowserHost objects
        // are reference counted, so you have a shared_ptr to your browserhost sometime. This
        // can be a big problem because BrowserHost manages a lot of your memory and if you get
        // a new one for each instances on each page (including page reloads).
        assert(BrowserHost::getInstanceCount() == 0);
        assert(PluginCore::getActivePluginCount() == 0);
        FB::Log::stopLogging();
    }
    return FW_SUCCESS;
}

WyrmColony::WyrmColony(FW_INST key) : m_key(key), m_threadId(std::this_thread::get_id()), m_nextSpawnId(1), m_nextCmdId(1), m_nextMethodId(1)
{
    initCommandMap();
}

WyrmColony::~WyrmColony()
{
    // First shut down any plugins which are still around
    for (auto inst : m_spawnMap) {
        WyrmSpawnPtr spawn = inst.second->spawn;
        spawn->shutdown();
    }
    m_spawnMap.clear();
}

AlienLarvaePtr WyrmColony::getLarvaeFor(const FW_INST spawnId, const uint32_t objId) {
    AlienLarvaePtr larvae;
    auto id = std::make_pair(spawnId, objId);
    auto fnd = m_larvaeMap.find(id);
    if (fnd != m_larvaeMap.end()) {
        larvae = fnd->second.lock();
    }
    if (!larvae) {
        larvae = std::make_shared<AlienLarvae>(this, spawnId, objId);
        m_larvaeMap[id] = larvae;
    }
    return larvae;
}

FB::variant valueToVariant(Json::Value root, WyrmColony* colony);

FB::variant valueRawObjectToVariant(Json::Value& root, WyrmColony *colony) {
    // If this is a "by value" object then we'll just pass it
    Json::Value def;
    Json::Value::Members members = root.getMemberNames();
    FB::VariantMap outMap;
    for (auto member : members) {
        outMap[member] = valueToVariant(root.get(member, def), colony);
    }
    return outMap;
}

FB::variant valueObjectToVariant(Json::Value& root, WyrmColony* colony) {
    assert(root.isObject());
    using ArrayIndex = decltype(root.size());

    if (root.isMember("$type")) {
        std::string type = root["$type"].asString();
        if (type == "ref" && root.isMember("data") && root["data"].isArray() && root["data"].size() >= 2) {
            // When objects are passed by reference from the page, we get here
            Json::Value data = root["data"];
            FW_INST spawnId = data[(ArrayIndex)0].asUInt();
            uint32_t objId = data[1].asUInt();
            return FB::variant(colony->getLarvaeFor(spawnId, objId), true);
        } else if (type == "ref" && root.isMember("data") && root["data"].isArray() && root["data"].size() >= 2) {
            // "local-ref" types refer to what are actually localWyrmlings here
            Json::Value data = root["data"];
            FW_INST spawnId = data[(Json::ArrayIndex)0].asUInt();
            uint32_t objId = data[1].asUInt();
            return FB::variant(WyrmlingKey(spawnId, objId), true);
        } else if (type == "json" && root.isMember("data")) {
            // "json" types are by-value data; in this case, convert it as though
            // no special firewyrm types existed and use it
            return FB::jsonValueToVariant(root["data"]);
        } else if (type == "binary") {
            // TODO: Handle binary properly
            return FB::variant();
        } else if (type == "error" && root.isMember("data")) {
            return root["data"].asString();
        }
        return valueRawObjectToVariant(root, colony);
    } else {
        return valueRawObjectToVariant(root, colony);
    }
}

FB::variant valueToVariant(Json::Value root, WyrmColony* colony) {
    Json::Value def;
    if (root.isString())
        return root.asString();
    else if (root.isBool())
        return root.asBool();
    else if (root.isDouble())
        return root.asDouble();
    else if (root.isInt())
        return root.asInt();
    else if (root.isUInt())
        return root.asUInt();
    else if (root.isNull())
        return FB::FBNull();
    else if (root.isArray()) {
        FB::VariantList outList;
        for (size_t i = 0; i < root.size(); ++i) {
            outList.push_back(valueToVariant(root.get(i, def), colony));
        }
        return outList;
    } else if (root.isObject()) {
        return valueObjectToVariant(root, colony);
    } else {
        return FB::FBVoid();
    }
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
    cFuncs->version = FW_VERSION;

    cFuncs->call = &FW_onCommand;
    cFuncs->cmdCallback = &FW_onCommandCallback;
}

using JsonSizeType = decltype(Json::Value().size());
FB::VariantList getArguments(WyrmColony* colony, Json::Value& root, JsonSizeType startIndex, JsonSizeType maxIndex = -1) {
    FB::VariantList args;

    // -1 in a uint is a really big number
    maxIndex = std::min(root.size(), maxIndex);
    for (decltype(root.size()) i = 1; i < maxIndex; ++i) {
        args.emplace_back(valueToVariant(root[i], colony));
    }

    return args;
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
        
        VariantList args = getArguments(this, root, 1, -1);
        try {
            auto dfd = (this->*cmd)(args);
            dfd.done([cmdId, this](FB::VariantList doc) {
                sendResponse(cmdId, doc);
            }, [cmdId, this](std::exception_ptr ep) {
                try {
                    if (ep) {
                        std::rethrow_exception(ep);
                    }
                } catch(const std::exception& e) {
                    sendResponse(cmdId, VariantList{ "error", VariantMap{ { "error", "Command threw an exception" }, { "message", e.what() } } });
                } catch(...) {}
                sendResponse(cmdId, VariantList{ "error", VariantMap{ { "error", "Command threw an exception" }, { "message", "Unknown message" } } });
            });
        } catch (std::exception& e) {
            sendResponse(cmdId, VariantList{ "error", VariantMap{ { "error", "Could not execute command" }, { "message", e.what() } } });
        }
    } else {
        return FW_ERR_INVALID_COMMAND;
    }
    
    return FW_ERR_UNKNOWN;
}

std::string stringify(Json::Value& root) {
    Json::FastWriter writer;
    std::string outString = writer.write(root);
    // Json::FastWriter leaves a \n at the end of the string, which causes issues with Chrome
    // We'll remove it
    outString[outString.length() - 1] = 0;
    outString.erase(outString.end() - 1);
    return outString;
}

void WyrmColony::sendResponse(const uint32_t cmdId, FB::VariantList resp) {
    auto outJSON = variantToJsonValue(resp);

    auto outDoc = stringify(outJSON);
    this->m_hFuncs.cmdCallback(m_key, cmdId, outDoc.c_str(), outDoc.size());
}

FW_RESULT WyrmColony::onResponse(const uint32_t cmdId, const std::string response) {
    auto fnd = m_waitMap.find(cmdId);
    if (fnd == m_waitMap.end()) {
        // Invalid cmd id; this must be a response to a command sent from this side
        return FW_ERR_INVALID_CMD_ID;
    }
    StringDeferred dfd{ fnd->second };
    dfd.resolve(response);
    return FW_SUCCESS;
}

WyrmColony::StringPromise WyrmColony::sendCommand(FB::VariantList cmd) {
    auto outJSON = variantToJsonValue(cmd);

    std::string outStr = stringify(outJSON);
    StringDeferred dfd;
    auto cmdId = getCommandId();
    m_waitMap[cmdId] = dfd;
    m_hFuncs.call(0, cmdId, outStr.c_str(), outStr.size());
    return dfd.promise();
}

FB::VariantListPromise WyrmColony::New(FB::VariantList args) {
    // Spawn a new instance
    FW_INST id = m_nextSpawnId++;
    std::string mimetype = args[0].convert_cast<std::string>();
    WyrmBrowserHostPtr wyrmHost{ std::make_shared<WyrmBrowserHost>(this, id) };
    auto wyrmSpawn = std::make_shared<WyrmSpawn>(wyrmHost, mimetype);
    try {
        wyrmSpawn->init(args[1].cast<FB::VariantMap>());
    } catch (...) {
        // No valid arguments passed in?
        wyrmSpawn->init(FB::VariantMap());
    }
    // wyrmHost->init() returns a Promise<void> which resolves when the browserhost is ready
    wyrmSpawn->setReady(wyrmHost->init());
    m_spawnMap[id] = std::make_shared<WyrmSac>(wyrmSpawn, wyrmHost);
    return FB::VariantList{ "success", id };
}

FB::VariantListPromise WyrmColony::Destroy(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();

    auto fnd = m_spawnMap.find(id);
    if (fnd != m_spawnMap.end()) {
        WyrmSpawnPtr spawn = fnd->second->spawn;
        spawn->shutdown();
        m_spawnMap.erase(fnd);
    } else {
        return FB::VariantList{ "error", FB::VariantMap{ { "error", "Invalid object" }, { "message", "Invalid object" } } };
    }

    return FB::VariantList{ "success", id };
}

FB::VariantListPromise WyrmColony::Enum(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();
    FW_INST objId = args[1].convert_cast<FW_INST>();

    if (id == 0) {
        // things in spawnId 0 have no members
        return FB::VariantList{ "success", FB::VariantList{} };
    }
    
    auto fnd = m_spawnMap.find(id);
    if (fnd != m_spawnMap.end()) {
        auto list = fnd->second->host->Enum(objId);
        return list.then<FB::VariantList>([](FB::VariantList members) -> FB::VariantList {
            return FB::VariantList{ "success", members };
        });
    } else {
        throw std::runtime_error("Invalid object");
    }
}

void evolveLarvae(FB::variant& in, WyrmBrowserHostPtr host, WyrmColony::SpawnMap& spawnMap) {
    if (in.is_of_type<AlienLarvaePtr>()) {
        in = AlienWyrmling::create(host, in.cast<AlienLarvaePtr>());
    } else if (in.is_of_type<WyrmlingKey>()) {
        // LocalWyrmling larvae
        auto key = in.cast<WyrmlingKey>();
        auto fnd = spawnMap.find(key.first);
        if (fnd == spawnMap.end()) {
            // This is not a valid spawn id; return a nullptr JSAPI
            in = FB::JSAPIPtr();
        } else {
            // Look up the localwyrmling and get the JSAPI object from it
            try {
                in = fnd->second->host->getJSAPIFromWyrmling(key.second);
            } catch (...) {
                // Generally this means it's not a valid wyrmling
                in = FB::JSAPIPtr();
            }
        }
    }
}

void evolveLarvae(FB::VariantList& in, WyrmBrowserHostPtr host, WyrmColony::SpawnMap& spawnMap) {
    for (auto &c : in) {
        evolveLarvae(c, host, spawnMap);
    }
}

void evolveLarvae(FB::VariantMap& in, WyrmBrowserHostPtr host, WyrmColony::SpawnMap& spawnMap) {
    for (auto &c : in) {
        evolveLarvae(c.second, host, spawnMap);
    }
}

FB::VariantListPromise WyrmColony::Invoke(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();
    FW_INST objId = args[1].convert_cast<FW_INST>();

    std::string name = args[2].convert_cast<std::string>();
    FB::VariantList invokeArgs = args[3].cast<FB::VariantList>();

    if (id == 0) {
        if (name.empty()) {
            // We are invoking a method object
            WyrmlingKey key(m_key, objId);
            auto fnd = localMethodMap.find(key);
            if (fnd != localMethodMap.end()) {
                // We found a method object to call
                auto ling = fnd->second.wyrmling;
                auto name = fnd->second.method;
                auto host = fnd->second.host;
                evolveLarvae(invokeArgs, host, m_spawnMap);
                auto res = host->Invoke(ling.getObjectId(), name, invokeArgs);
                return res.then<FB::VariantList>([host](FB::variant res) -> FB::VariantList {
                    return FB::VariantList{ "success", preprocessVariant(res, host) };
                });
            } else {
                throw std::runtime_error("Invalid object");
            }
        } else {
            throw invalid_member(name);
        }
    } else {
        auto fnd = m_spawnMap.find(id);
        if (fnd != m_spawnMap.end()) {
            auto host = fnd->second->host;
            evolveLarvae(invokeArgs, host, m_spawnMap);
            auto res = host->Invoke(objId, name, invokeArgs);
            return res.then<FB::VariantList>([host](FB::variant res) -> FB::VariantList {
                return FB::VariantList{ "success", preprocessVariant(res, host) };
            });
        } else {
            throw std::runtime_error("Invalid object");
        }
    }
}

FB::variant WyrmColony::makeLocalMethodWyrmling(WyrmBrowserHostPtr host, LocalWyrmling wyrmling, std::string method) {
    LocalMethodWyrmling methodWyrm(wyrmling, method, host);
    FW_INST newObjId = m_nextMethodId++;
    // There is a (very) remote chance of creating so many objects that we overflow; in that case
    // we could reuse a used id. I'm conciously deciding not to address that until I find that it is happening;
    // after all, 32 bits is a *lot* of method objects.
    
    WyrmlingKey key(m_key, newObjId);
    localMethodMap[key] = methodWyrm;
    
    return FB::variant(WyrmlingKey(0, newObjId), true);
}

FB::VariantListPromise WyrmColony::GetP(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();
    FW_INST objId = args[1].convert_cast<FW_INST>();

    std::string name = args[2].convert_cast<std::string>();

    if (id == 0) {
        // Can't setP for anything in spawnId 0...
        throw invalid_member(name);
    }
    auto fnd = m_spawnMap.find(id);
    if (fnd != m_spawnMap.end()) {
        // The spawn exists; check to see if this is a method call
        auto host = fnd->second->host;
        if (host->HasMethod(objId, name)) {
            // This is a valid object and it's a method call, so create a
            // local method wyrmling
            return FB::VariantList{ "success", preprocessVariant(makeLocalMethodWyrmling(host, host->getWyrmling(objId), name), host) };
        }
        auto res = host->GetP(objId, name);
        return res.then<FB::VariantList>([host](FB::variant res) -> FB::VariantList {
            return FB::VariantList{ "success", preprocessVariant(res, host) };
        });
    } else {
        throw std::runtime_error("Invalid object");
    }
}

FB::VariantListPromise WyrmColony::SetP(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();
    FW_INST objId = args[1].convert_cast<FW_INST>();

    std::string name = args[2].convert_cast<std::string>();
    FB::variant newVal = args[3];

    if (id == 0) {
        // Can't setP for anything in spawnId 0...
        throw invalid_member(name);
    }
    auto fnd = m_spawnMap.find(id);
    if (fnd != m_spawnMap.end()) {
        evolveLarvae(newVal, fnd->second->host, m_spawnMap);
        auto res = fnd->second->host->SetP(objId, name, newVal);
        return res.then<FB::VariantList>([]() -> FB::VariantList {
            return FB::VariantList{ "success", FB::FBNull() };
        });
    } else {
        throw std::runtime_error("Invalid object");
    }
}

FB::VariantListPromise WyrmColony::RelObj(FB::VariantList args) {
    FW_INST id = args[0].convert_cast<FW_INST>();
    FW_INST objId = args[1].convert_cast<FW_INST>();

    if (id == 0) {
        // This is a local methodMap
        auto fnd = localMethodMap.find(WyrmlingKey(m_key, objId));
        if (fnd != localMethodMap.end()) {
            localMethodMap.erase(fnd);
            return FB::VariantList{ "success", FB::FBNull() };
        } else {
            throw std::runtime_error("Invalid object");
        }
    } else {
        auto fnd = m_spawnMap.find(id);
        if (fnd != m_spawnMap.end()) {
            auto res = fnd->second->host->RelObj(objId);
            return res.then<FB::VariantList>([]() -> FB::VariantList {
                return FB::VariantList{ "success", FB::FBNull() };
            });
        } else {
            throw std::runtime_error("Invalid object");
        }
    }
}

bool WyrmColony::_scheduleAsyncCall(void(*func)(void*), void * userData) {
    return m_hFuncs.doAsyncCall(func, userData) == 0;
}

FB::variantPromise WyrmColony::DoCommand(FB::VariantList args, WyrmBrowserHostPtr host) {
    if (host) {
        for (auto& arg : args) {
            arg = preprocessVariant(arg, host);
        }
    }
    auto promise = sendCommand(args);
    return promise.then<FB::variant>([this, host](std::string resp) -> FB::variant {
        // This is where the magic happens... =]
        Json::Reader rdr;
        Json::Value root;
        
        int statusIdx{ 0 };
        if (!rdr.parse(resp, root, false)) {
            throw FB::script_error("Malformed JSON from client");
        } else if (!root.isArray() || root.size() < 1 || !root[statusIdx].isString()) {
            throw FB::script_error("Invalid message format");
        }

        std::string status(root[statusIdx].asString());

        if (status == "error") {
            auto obj = root.get(1, Json::objectValue);
            if (obj.isMember("message")) {
                throw FB::script_error(obj.get("message", "").asString());
            } else {
                throw FB::script_error("Unknown error returned from page: " + resp);
            }
        } else if (status == "success") {
            FB::variant resp = valueToVariant(root.get(1, Json::nullValue), this);
            if (host) {
                if (resp.is_of_type<FB::VariantList>()) {
                    FB::VariantList vl = resp.cast<FB::VariantList>();
                    evolveLarvae(vl, host, m_spawnMap);
                    resp = vl;
                } else if (resp.is_of_type<FB::VariantMap>()) {
                    FB::VariantMap vm = resp.cast<FB::VariantMap>();
                    evolveLarvae(vm, host, m_spawnMap);
                    resp = vm;
                } else {
                    evolveLarvae(resp, host, m_spawnMap);
                }
            }
            return resp;
        } else {
            throw FB::script_error("Invalid response from client");
        }
    });
}
