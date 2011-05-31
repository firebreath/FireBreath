/**********************************************************\ 
Original Author: Dan Weatherford

Imported into FireBreath:    Oct 4, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Weatherford and Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_HTTP_HTTPREQUEST
#define H_HTTP_HTTPREQUEST

#include "../HTTPCommon/HTTPRequestData.h"
#include "../HTTPCommon/HTTPResponseData.h"
#include "../HTTPCommon/HTTPProxyConfig.h"
#include "../HTTPCommon/Status.h"
#include <boost/thread.hpp>

#undef ERROR // windows...
#undef Status // unix!

typedef void CURL;

namespace HTTP {
    class HTTPRequest : public boost::noncopyable {
        public:
            static HTTPRequest* create();
            static void setProxyConfig(const HTTPProxyConfig& cfg);
            static void registerCACert(const std::string& cert);

            // Fire off a request in a new thread and ignore the results with this.
            static void asyncStartRequest(boost::shared_ptr<HTTPRequestData> data);

            // If you need to kill an HTTP request from a thread whose callstack might go through that
            // request's IO service, then call this; it'll transfer ownership to a new thread which will
            // stop the service and join the worker thread before deleting the object.
            void threadSafeDestroy();

            ~HTTPRequest();

            void startRequest(boost::shared_ptr<HTTPRequestData> data);
            boost::shared_ptr<HTTPRequestData> getRequest();
            boost::shared_ptr<HTTPResponseData> getResponse();
            void cancel();
            void awaitCompletion();

            Status getStatus() const;
            typedef boost::function<void(Status)> callback_fn_t;
            void onStatusChanged(const callback_fn_t& _callback);

            // internal only below here
            int curl_progress(double dltotal, double dlnow, double ultotal, double ulnow);
            void curl_header(const char* data, size_t size);

        protected:

            HTTPRequest();
            void startRequest_thread();
            void killThread();
            void _internal_threadSafeDestroy();

            CURL* req;
            bool cancellation_requested;

            Status last_status;
            callback_fn_t status_callback;

            boost::shared_ptr<boost::thread> worker_thread;
            boost::shared_ptr<HTTPRequestData> request_data;
            boost::shared_ptr<HTTPResponseData> response_data;
            HTTPProxyConfig proxy_config;

            static HTTPProxyConfig static_proxy_config;
    };
}; 

#endif // H_HTTP_HTTPREQUEST

