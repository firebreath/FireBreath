/**********************************************************\
Original Author: Richard Bateman

Created:    Oct 4, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook Inc, Firebreath development team
\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "SafeQueue.h"
#include <boost/thread/thread.hpp>

#pragma once
#ifndef H_THREADRUNNERAPI
#define H_THREADRUNNERAPI

FB_FORWARD_PTR(FBTestPlugin)

class ThreadRunnerAPI : public FB::JSAPIAuto
{
public:
    ThreadRunnerAPI(const FB::BrowserHostPtr& host, const FBTestPluginWeakPtr& plugin);
    virtual ~ThreadRunnerAPI();

    void addMethod(const FB::JSObjectPtr &);
    void addRequest(const std::string& url, const FB::JSObjectPtr &);
protected:
    void threadRun();

    FBTestPluginPtr getPlugin();

protected:
    FBTestPluginWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
    boost::thread m_thread;
    FB::SafeQueue<FB::JSObjectPtr> m_queue;
    FB::SafeQueue<std::pair<std::string, FB::JSObjectPtr> > m_UrlRequestQueue;

    std::string m_testString;
};

#endif

