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

#include "FBTestPlugin.h"
#include "SimpleStreamHelper.h"
#include "variant_list.h"
#include <utility>
#include "logging.h"

#include "ThreadRunnerAPI.h"

ThreadRunnerAPI::ThreadRunnerAPI(const FB::BrowserHostPtr& host, const FBTestPluginWeakPtr& plugin)
    : m_plugin(plugin), m_host(host)
{
    registerMethod("addMethod", make_method(this, &ThreadRunnerAPI::addMethod));
    registerMethod("addRequest", make_method(this, &ThreadRunnerAPI::addRequest));

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

        try {
            // This serves no real useful purpose; it forces the window to redraw.
            // Really this is here to make sure the cross-thread invalidate works
            FB::PluginWindow* win = getPlugin()->GetWindow();
            if (win)
                win->InvalidateWindow();
        } catch (const FB::script_error&) {
            // do nothing
        }
        
        // This is both insecure (allowing illegal xss) and inefficient, since
        // we could just start the request from the main thread; however, the
        // purpose is to test that synchronous calls even work from this thread

        std::pair<std::string, FB::JSObjectPtr> val;
        if (m_UrlRequestQueue.try_pop(val)) {
            FBLOG_WARN("ThreadRunner", "Beginning request of " << val.first);
            FB::HttpStreamResponsePtr ret = FB::SimpleStreamHelper::SynchronousPost(m_host,
                FB::URI::fromString(val.first), "");
            FBLOG_WARN("ThreadRunner", "Finished request of " << val.first << " and got " << ret->success);
            FB::VariantMap outHeaders;
            for (FB::HeaderMap::const_iterator it = ret->headers.begin(); it != ret->headers.end(); ++it) {
                if (ret->headers.count(it->first) > 1) {
                    if (outHeaders.find(it->first) != outHeaders.end()) {
                        outHeaders[it->first].cast<FB::VariantList>().push_back(it->second);
                    } else {
                        outHeaders[it->first] = FB::VariantList(FB::variant_list_of(it->second));
                    }
                } else {
                    outHeaders[it->first] = it->second;
                }
            }
            if (ret->success) {
                std::string dstr(reinterpret_cast<const char*>(ret->data.get()), ret->size);
                val.second->InvokeAsync("", FB::variant_list_of(ret->success)(outHeaders)(dstr));
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

void ThreadRunnerAPI::addRequest( const std::string& url, const FB::JSObjectPtr &obj )
{
    m_UrlRequestQueue.push(std::make_pair(url, obj));
}

FBTestPluginPtr ThreadRunnerAPI::getPlugin()
{
    FBTestPluginPtr ptr(m_plugin.lock());
    if (!ptr) {
        throw FB::script_error("Plugin closed");
    }
    return ptr;
}

