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

class ThreadRunnerAPI : public FB::JSAPIAuto
{
public:
    ThreadRunnerAPI(FB::BrowserHostPtr host);
    virtual ~ThreadRunnerAPI();

    void addMethod(const FB::JSObjectPtr &);
protected:
    void threadRun();

protected:
    FB::BrowserHostPtr m_host;
    boost::thread m_thread;
    FB::SafeQueue<FB::JSObjectPtr> m_queue;

    std::string m_testString;
};

#endif
