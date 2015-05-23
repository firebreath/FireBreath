/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 17, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include <cmath>
#include <algorithm>
#include <sstream>
#include "MainLoop.h"
#include "json/json.h"

// The maximum size when this was written for a message from the plugin to the browser was 1MB; to avoid
// ever hitting this limit, we're going to use 768K as our max size just in case
const int maxCommandSize = 768 * 1024;

inline size_t countChunks(double size) {
    return static_cast<size_t>(ceil(size / maxCommandSize));
}

std::map<uint32_t, messageInfo> msgMap;

std::string stringify(Json::Value& root) {
    Json::FastWriter writer;
    std::string outString = writer.write(root);
    // Json::FastWriter leaves a \n at the end of the string, which causes issues with Chrome
    // We'll remove it
    outString[outString.length() - 1] = 0;
    outString.erase(outString.end() - 1);
    return outString;
}

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

FW_RESULT doAsyncCall(const FW_AsyncCall call, void* pData) {
    MainLoop::get().scheduleCall(call, pData);
    return FW_SUCCESS;
}

/* Example:
{
    "cmd": "create",
    "mimetype": "application/x-fbtestplugin"
}
*/
messageInfo parseCommandMessage(Json::Value& root) {
    std::string cmd = root["cmd"].asString();
    if (cmd == "create") {
        if (!root.isMember("mimetype")) {
            throw std::runtime_error("Missing Mimetype");
        }
        messageInfo msg;
        msg.type = MessageType::CREATE;
        msg.msgs.emplace_back(root["mimetype"].asString());
        msg.curC++;
        return msg;
    } else if (cmd == "destroy") {
        messageInfo msg;
        msg.type = MessageType::DESTROY;
        msg.c = 0;
        return msg;
    } else if (cmd == "list") {
        messageInfo msg;
        msg.type = MessageType::ENUM;
        msg.c = 0;
        return msg;
    } else {
        throw std::runtime_error("Unknown command");
    }
}

/* Example:
{
    "cmdId": 1,
    "c": 1,
    "n": 1,
    "colonyId": 0,
    "msg": "[\"New\", \"application/x-fbtestplugin\", {}]"
}
*/
messageInfo parseWyrmholeMessage(Json::Value& root) {
    if (!root.isMember("c") || !root["c"].isIntegral() || !root.isMember("cmdId") || !root["cmdId"].isIntegral()) {
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
    std::string typeStr;
    if (root.isMember("type")) {
        typeStr = root["type"].asString();
    }
    MessageType type = typeStr == "resp" ?
        MessageType::RESPONSE : MessageType::COMMAND;

    messageInfo info(getMessageInfo(cmdId, c));
    info.colonyId = colonyId;
    info.msgs[n] = root["msg"].asString();
    info.curC++;
    info.type = type;
    return info;
}

messageInfo parseIncomingMessage(std::string command) {
    Json::Reader rdr;
    Json::Value root;

    if (!rdr.parse(command, root, false)) {
        throw std::runtime_error("Invalid json");
    } else if (!root.isObject()) {
        throw std::runtime_error("Invalid message");
    }

    if (root.isMember("cmd")) {
        return parseCommandMessage(root);
    } else if (root.isMember("msg")) {
        return parseWyrmholeMessage(root);
    } else {
        throw std::runtime_error("Unknown message");
    }
    
}

void MainLoop::messageIn(std::string msg) {
    // Process the message before we send it in
    messageInfo processedMsg;
    try {
        processedMsg = parseIncomingMessage(msg);
    } catch (std::exception& e) {
        processedMsg = messageInfo(MessageType::ERROR, e.what());
    }

    if (processedMsg.isComplete()) {
        std::unique_lock<std::mutex> _l(m_mutex);
        m_messagesIn.emplace_back(processedMsg);
        _l.unlock();
        m_cond.notify_all();
    }
}

FW_RESULT sendCommand(const FW_INST colonyId, const uint32_t cmdId, const char* strCommand, uint32_t strCommandLen, std::string type) {
    MainLoop& main(MainLoop::get());

    Json::Value root(Json::objectValue);
    uint32_t c = countChunks(strCommandLen);
    root["c"] = c;
    root["type"] = type;
    root["colonyId"] = colonyId;
    root["cmdId"] = cmdId;
    
    for (uint32_t i = 0; i < c; ++i) {
        root["n"] = i + 1;
        root["msg"] = std::string(strCommand + (maxCommandSize * i), std::min((int)maxCommandSize, (int)(strCommandLen - maxCommandSize * i)));

        main.writeMessage(stringify(root));
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
	log("Starting main message loop");

    memset(&m_cFuncs, 0, sizeof(m_cFuncs));
    m_cFuncs.size = sizeof(m_cFuncs);
    m_cFuncs.version = FW_VERSION;

    m_hFuncs.size = sizeof(m_hFuncs);
    m_hFuncs.version = FW_VERSION;
    m_hFuncs.doAsyncCall = &doAsyncCall;
    m_hFuncs.call = &doCommand;
    m_hFuncs.cmdCallback = &doCommandCallback;

    std::unique_lock<std::mutex> _l(m_mutex);
    while (!m_needsToExit) {
		waitForWork(&_l);

        if (m_needsToExit) {
            // TODO: Do any cleanup here
            return;
        }

        // This is intentionally not a loop; this way new calls scheduled will not prevent other messages
        // from being processed
        if (m_AsyncCalls.size()) {
            AsyncCall c = m_AsyncCalls.front();
            m_AsyncCalls.pop_front();

            // Unlock the mutex while we do the call so other messages can be scheduled in the mean time
            // This also prevents deadlocks if the async call is reentrant, since we aren't using a recursive
            // lock
            _l.unlock();
            (*(c.fn))(c.pData);
            _l.lock();
        }
        if (m_messagesIn.size()) {
            messageInfo message = m_messagesIn.front();
            m_messagesIn.pop_front();

            _l.unlock();
            processBrowserMessage(message);
            _l.lock();
        }

        // TODO: Handle commands that need to go out
        // TODO: Handle commands that came in from the ReadLoop
    }
}

void MainLoop::writeObj(stringMap outMap, messageInfo msg) {
    Json::Value v{ Json::objectValue };
    for (auto c : outMap) {
        v[c.first] = c.second;
    }
    if (msg.msgId) {
        v["cmdId"] = msg.msgId;
        v["c"] = 1;
        v["n"] = 1;
    }
    this->writeMessage(stringify(v));
}

void MainLoop::processBrowserMessage(messageInfo& message) {
    if (message.type == MessageType::ERROR) {
        writeObj(stringMap{ { "status", "error" }, { "message", message.msgs[0] } });
    } else if (message.type == MessageType::CREATE) {
        try {
            m_pluginLoader = PluginLoader::LoadPlugin(message.msgs[0]);

            m_pluginLoader->Init(&m_hFuncs, &m_cFuncs);
            writeObj(stringMap{ { "status", "success" }, { "plugin", m_pluginLoader->getPluginName() } });
        } catch (std::exception& e) {
            writeObj(stringMap{ { "status", "error" }, { "message", e.what() } });
        }
    } else if (message.type == MessageType::DESTROY) {
        try {
            if (m_pluginLoader) {
                m_pluginLoader->Finit();
            }
            m_pluginLoader.reset();
            writeObj(stringMap{ { "status", "success" } });
        } catch (std::exception& e) {
            writeObj(stringMap{ { "status", "error" }, { "message", e.what() } });
        }
    } else if (message.type == MessageType::ENUM) {
        // Enumerate the plugins installed
        try {
            auto list = PluginLoader::getPluginList();

            Json::Value plugArr{ Json::arrayValue };
            for (auto& plugin : list) {
                Json::Value pl{ Json::objectValue };
                pl["name"] = plugin.name;
                pl["description"] = plugin.description;
                pl["product"] = plugin.product_name;
                pl["vendor"] = plugin.vendor;
                pl["version"] = plugin.version;
                Json::Value types{ Json::arrayValue };
                for (auto strMimetype : plugin.mime_types) {
                    types.append(strMimetype);
                }
                pl["mimetypes"] = types;
                plugArr.append(pl);
            }
            Json::Value msg{ Json::objectValue };
            msg["status"] = "success";
            msg["list"] = plugArr;
            writeMessage(stringify(msg));
        } catch (std::exception& e) {
            writeObj(stringMap{ { "status", "error" }, { "message", e.what() } });
        }
    } else {
        if (message.type == MessageType::COMMAND) {
            std::string msg = message.getString();
            if (!m_pluginLoader) {
                writeObj(stringMap{{ "status", "error" }, { "message", "Plugin not loaded" }}, message);
                return;
            }
            (*m_cFuncs.call)(message.colonyId, message.msgId, msg.c_str(), msg.size());
        } else if (message.type == MessageType::RESPONSE) {
            std::string msg = message.getString();
            if (!m_pluginLoader) {
                writeObj(stringMap{ { "status", "error" }, { "message", "Plugin not loaded" } }, message);
                return;
            }
            (*m_cFuncs.cmdCallback)(message.colonyId, message.msgId, msg.c_str(), msg.size());
        } else {
            writeObj(stringMap{ { "status", "error" }, { "message", "Unknown message" } }, message);
        }
    }
}
