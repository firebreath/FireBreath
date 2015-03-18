/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 17, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include <cmath>
#include <algorithm>
#include <sstream>
#include "MainLoop.h"
#include "PluginLoader.h"
#include "json/json.h"

// The maximum size when this was written for a message from the plugin to the browser was 1MB; to avoid
// ever hitting this limit, we're going to use 768K as our max size just in case
const int maxCommandSize = 768 * 1024;

inline size_t countChunks(double size) {
    return static_cast<size_t>(ceil(size / maxCommandSize));
}


std::map<uint32_t, messageInfo> msgMap;

messageInfo& getMessageInfo(uint32_t msgId, size_t c) {
    auto fnd = msgMap.find(msgId);
    if (fnd == msgMap.end()) {
        messageInfo newMsg(c, msgId);
        msgMap[msgId] = newMsg;
        return msgMap[msgId];
    } else {
        if (fnd->second.c != c) {
            throw std::runtime_error("Invalid sequence size; sequence size already set");
        }
        return fnd->second;
    }
}

FW_RESULT doAsyncCall(const FW_AsyncCall call, const void* pData) {
    MainLoop::get().scheduleCall(call, pData);
    return FW_SUCCESS;
}

messageInfo parseString(const char* strCommand, uint32_t strCommandLen) {
    Json::Reader rdr;
    Json::Value root;
    std::string command(strCommand, strCommandLen);

    int cmdIdx{ 0 };
    if (!rdr.parse(command, root, false)) {
        throw std::runtime_error("Invalid json");
    } else if (!root.isObject() || !root.isMember("msg") || !root.isMember("c") || !root["c"].isIntegral() || !root.isMember("cmdId") || !root["cmdId"].isIntegral()) {
        throw std::runtime_error("Invalid message");
    }
    FW_INST colonyId = 0;
    if (root.isMember("colonyId")) {
        colonyId = root.get("colonyId", 0).asUInt();
    }
    uint32_t cmdId = root["cmdId"].asUInt();
    auto c = root.get("c", 1).asUInt();
    auto n = c-1;
    // Extremely unlikely to be needed on this side, but let's be symmetrical
    if (c > 1) {
        if (!root.isMember("n") || !root["n"].isIntegral()) {
            throw std::runtime_error("Missing sequence id in multi-part message");
        }
        n = root.get("n", 1).asUInt() - 1;
    }

    messageInfo info(getMessageInfo(c, cmdId));
    info.colonyId = colonyId;
    info.msgs[n] = root["msg"].asString();
    info.curC++;
    return info;
}

FW_RESULT sendCommand(const FW_INST colonyId, const uint32_t cmdId, const char* strCommand, uint32_t strCommandLen, std::string type) {
    MainLoop& main(MainLoop::get());

    Json::Value root(Json::objectValue);
    auto c = countChunks(strCommandLen);
    root["c"] = c;
    root["type"] = type;
    root["colonyId"] = colonyId;
    root["cmdId"] = cmdId;
    
    for (size_t i = 0; i < c; ++i) {
        root["n"] = i + 1;
        root["msg"] = std::string(strCommand + (maxCommandSize * i), std::min((int)maxCommandSize, (int)(strCommandLen - maxCommandSize * i)));

        std::ostringstream out;
        out << root;
        main.writeMessage(out.str());
    }

    return FW_SUCCESS;
}

FW_RESULT doCommand(const FW_INST colonyId, const uint32_t cmdId, const char* strCommand, uint32_t strCommandLen) {
    return sendCommand(colonyId, cmdId, strCommand, strCommandLen, "cmd");
}

FW_RESULT doCommandCallback(const FW_INST colonyId, const uint32_t cmdId, const char* strResp, uint32_t strRespLen) {
    return sendCommand(colonyId, cmdId, strResp, strRespLen, "resp");
}

MainLoop& MainLoop::get(std::string url) {
    static MainLoop main(url, PrivateStruct());
    return main;
}

void MainLoop::run() {
    // This is the main loop

    FWHostFuncs hFuncs;
    hFuncs.size = sizeof(hFuncs);
    hFuncs.version = FW_VERSION;
    hFuncs.doAsyncCall = &doAsyncCall;
    hFuncs.call = &doCommand;
    hFuncs.cmdCallback = &doCommandCallback;

    memset(&m_cFuncs, 0, sizeof(m_cFuncs));
    m_cFuncs.size = sizeof(m_cFuncs);
    m_cFuncs.version = FW_VERSION;



    auto workExists = [this]() {
        return m_needsToExit || m_messagesIn.size() || m_AsyncCalls.size();
    };

    std::unique_lock<std::mutex> _l(m_mutex);
    while (!m_needsToExit) {
        m_cond.wait(_l, workExists);
        if (m_needsToExit) {
            // TODO: Do any cleanup here
            return;
        }
    }
}

void MainLoop::writeMessage(std::string output) {
    auto a = output.size();
    std::cout << char(((a >> 0) & 0xFF))
              << char(((a >> 8) & 0xFF))
              << char(((a >> 16) & 0xFF))
              << char(((a >> 24) & 0xFF))
              << output;
}
