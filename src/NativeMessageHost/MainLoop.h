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

#pragma once
#ifndef MainLoop_h__
#define MainLoop_h__

#include <string>
#include <thread>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "FireWyrm.h"

class PluginLoader;

struct messageInfo
{
    messageInfo(size_t c, uint32_t msgId) : msgId(msgId), c(c), curC(0), msgs(c) {}
    messageInfo() {}
    FW_INST colonyId;
    uint32_t msgId;
    std::string command;
    size_t c;
    size_t curC;
    std::vector<std::string> msgs;

    std::string getString() {
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
        AsyncCall(FW_AsyncCall fn, const void* pData) : fn(fn), pData(pData) {}
        FW_AsyncCall fn;
        const void* pData;
    };
public:
    MainLoop(std::string url, PrivateStruct) : m_url(url), m_needsToExit(false) {}
    static MainLoop& get(std::string url = "");

    void run();

    void messageIn(std::string msg) {
        std::unique_lock<std::mutex> _l(m_mutex);
        m_messagesIn.emplace_back(msg);
        _l.unlock();
        m_cond.notify_all();
    }
    void scheduleCall(FW_AsyncCall fn, const void* pData) {
        std::unique_lock<std::mutex> _l(m_mutex);
        m_AsyncCalls.emplace_back(AsyncCall(fn, pData));
        _l.unlock();
        m_cond.notify_all();
    }
    void writeMessage(std::string msg);

private:
    FW_RESULT processMessage(messageInfo msg);
    std::deque<std::string> m_messagesIn;
    std::deque<AsyncCall> m_AsyncCalls;
    std::string m_url;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_needsToExit;
    std::unique_pointer<PluginLoader> m_pluginLoader;
    FWColonyFuncs m_cFuncs;
};

#endif // MainLoop_h__