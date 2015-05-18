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

#pragma once
#ifndef MainLoop_h__
#define MainLoop_h__

#include <string>
#include <string.h>
#include <deque>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include "FireWyrm.h"
#include "PluginLoader.h"

enum class MessageType { UNKNOWN, CREATE, DESTROY, COMMAND, RESPONSE, ERROR, ENUM };

using stringMap = std::map<std::string, std::string>;

struct messageInfo
{
    messageInfo(size_t c, uint32_t msgId) : colonyId(0), msgId(msgId), c(c), curC(0), msgs(c) {}
    messageInfo(MessageType type, std::string msg) : type(type) {
        msgs.emplace_back(msg);
    }
    messageInfo() : msgId(0), c(1), curC(0) {}

    FW_INST colonyId;
    uint32_t msgId;
    size_t c;
    size_t curC;
    std::vector<std::string> msgs;

    MessageType type;
    
    bool isComplete() const {
        return curC >= c;
    }

    std::string getString() const {
        std::string out;
        size_t len = 0;
        for (auto m : msgs) {
            len += m.size();
        }
        out.reserve(len); // try to be a bit more memory efficient
        for (auto m : msgs) {
            out += m;
        }
        return out;
    }
};
class MainLoop
{
private:
    struct PrivateStruct {};
    struct AsyncCall
    {
        AsyncCall(FW_AsyncCall fn, void* pData) : fn(fn), pData(pData) {}
        FW_AsyncCall fn;
        void* pData;
    };
public:
    MainLoop(std::string url, PrivateStruct) : m_url(url), m_needsToExit(false) {}
    static MainLoop& get(std::string url = "");

    void run();
	void waitForWork(std::unique_lock<std::mutex> *_l);

    void messageIn(std::string msg);
    void scheduleCall(FW_AsyncCall fn, void* pData) {
        std::unique_lock<std::mutex> _l(m_mutex);
        m_AsyncCalls.emplace_back(AsyncCall(fn, pData));
        _l.unlock();
        m_cond.notify_all();
    }
    void writeObj(stringMap outMap, messageInfo msg = messageInfo());
    void writeMessage(std::string output) {
        uint32_t a = output.size(); // strip off newline
        fwrite(&a, sizeof(a), 1, stdout);
        fwrite(output.data(), sizeof(char), a, stdout);
        fflush(stdout);
    }
    void shutdown() {
        std::unique_lock<std::mutex> _l(m_mutex);
		log("Exit signaled");
        m_needsToExit = true;
        _l.unlock();
        m_cond.notify_all();
    }

private:
    FW_RESULT processMessage(messageInfo msg);
    void processBrowserMessage(messageInfo& message);
    std::deque<messageInfo> m_messagesIn;
    std::deque<AsyncCall> m_AsyncCalls;
    std::string m_url;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_needsToExit;
    std::unique_ptr<PluginLoader> m_pluginLoader;
    FWColonyFuncs m_cFuncs;
    FWHostFuncs m_hFuncs;
};

#endif // MainLoop_h__