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
#ifndef H_HTTP_BASICSERVICE
#define H_HTTP_BASICSERVICE

#include "win_targetver.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <vector>
#include <map>
#include "Countable.h"
#include "../HTTPCommon/Tiger.h"
#include "URI.h"
#include "../HTTPCommon/HTTPDatablock.h"
#include "../HTTPService.h"
#include "HTTPHandler.h"
#include "../HTTPCommon/HTTPRequestData.h"
#include "../HTTPCommon/HTTPResponseData.h"

namespace HTTP {
    class BasicService : public HTTPService {
    public:
        friend class HTTPService;

    protected:
        BasicService(const std::string &ipaddr, const int port, const std::string &hostname);
    public:
        virtual ~BasicService();

        void terminate();

        void registerHandler(boost::shared_ptr<HTTPHandler> hnd);
        void deregisterHandler(boost::shared_ptr<HTTPHandler> hnd);

        // While this is enabled, the service will hold a reference to itself that it will release when
        // the builtin URI "/shutdown" is accessed.
        void setDeferShutdown(bool val);

        FB::URI getBaseUri() const;
        // Add a signature to a URI (via a GET param). Only the path is signed, so you can still
        // change GET params however you like without causing the verification to fail.
        void sign_uri(FB::URI& in_uri) const;

    protected:
        static const size_t threadpool_size = 2;

    protected:
        void init();

        void _worker_thread_entry();
        void do_async_accept();
        bool check_uri_signature(const FB::URI& in_url);

        class Session : public Countable {
        public:
            typedef boost::intrusive_ptr<Session> ptr;

            Session(boost::asio::io_service& svc);
            ~Session();

            void start(const boost::shared_ptr<BasicService>& _parent_svc);

            boost::asio::ip::tcp::socket& socket() { return sock; }
        protected:
            void wait_for_header();
            void handle_request(boost::system::error_code ec);
            void handle_response_datablock_complete(boost::system::error_code ec, HTTPResponseData* resp);

            boost::asio::ip::tcp::socket sock;
            boost::asio::streambuf data;
            boost::shared_ptr<BasicService> parent_svc;
        };
        friend class HTTP::BasicService::Session;

        void handle_accept(const boost::system::error_code& ec, Session::ptr socket);
        std::string tiger_hmac(const std::string& sign_str) const;
        // -- data
        char* signing_key;
        size_t signing_key_length;
        boost::weak_ptr<BasicService> _weak_ref;
        std::list<boost::shared_ptr<HTTPHandler> > handlers;

        boost::shared_ptr<BasicService> deferred_shutdown_ref;

        std::vector<boost::shared_ptr<boost::thread> > threadpool;
        boost::asio::io_service service;

        boost::asio::ip::tcp::acceptor srv_acceptor;
        boost::asio::ip::tcp::endpoint srv_endpoint;
        std::string m_hostname;
    };
};

#endif // H_HTTP_BASICSERVICE

