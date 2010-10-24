/*
 *  HTTPRequest.h
 *  fbplugin
 *
 */

#pragma once
#include "../Common/windows_defs.h"
#include "../Common/HTTPRequestData.h"
#include "../Common/HTTPResponseData.h"
#include "../Common/HTTPProxyConfig.h"
#include <boost/thread.hpp>

//#include "PluginJSDict.h"

#undef ERROR // windows...
#undef Status // unix!

typedef void CURL;

class HTTPRequest : public boost::noncopyable {
    public:
        static HTTPRequest* create();
        static void setProxyConfig(const HTTPProxyConfig& cfg);

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

        struct Status {
            //PluginJSDict* asDict() const;
            enum State {
                CANCELLED = -2,
                ERROR = -1,
                IDLE = 0,
                RESOLVING = 1,
                CONNECTING = 2,
                SEND_REQUEST = 3,
                READ_RESPONSE = 4,
                COMPLETE = 5,
            };
            State state;
            size_t bytes_sent;
            size_t send_total;
            size_t bytes_received;
            size_t receive_total;
            double bytes_per_second_send;
            double bytes_per_second_receive;
            std::string last_error;

            Status() : state(IDLE), bytes_sent(0), send_total(0), bytes_received(0), receive_total(0), bytes_per_second_send(0), bytes_per_second_receive(0) {}
        };
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
