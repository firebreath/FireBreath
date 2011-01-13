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

#include "ThreadRunnerAPI.h"

ThreadRunnerAPI::ThreadRunnerAPI(FB::BrowserHostPtr host) : m_host(host)
{
    registerMethod("addMethod", make_method(this, &ThreadRunnerAPI::addMethod));

    m_thread = boost::thread(&ThreadRunnerAPI::threadRun, this);
}

void ThreadRunnerAPI::threadRun()
{
    while (!boost::this_thread::interruption_requested())
    {
        m_host->htmlLog("Thread Dialog iteration start");

        FB::JSObjectPtr func;
        if (this->m_queue.try_pop(func))
        {
            FB::variant var;
            try {
                var = func->Invoke("", FB::VariantList());
            } catch (const FB::script_error& ex) {
                // The function call failed
                m_host->htmlLog(std::string("Function call failed with ") + ex.what());
                continue;
            }
            if (var.is_of_type<std::string>()) {
                m_host->htmlLog("Function call returned: " + var.convert_cast<std::string>());
            } else if (var.is_of_type<FB::JSObjectPtr>()) {
                m_queue.push(var.convert_cast<FB::JSObjectPtr>());
            }
        }
        
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
}

ThreadRunnerAPI::~ThreadRunnerAPI()
{
    m_thread.interrupt();
    m_thread.join();
}

void ThreadRunnerAPI::addMethod(const FB::JSObjectPtr &obj)
{
    m_queue.push(obj);
}
