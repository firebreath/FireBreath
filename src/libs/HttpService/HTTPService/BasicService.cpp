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

#include "../HTTPCommon/base64.h"
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/nondet_random.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include "Platform/Platform.h"
#include "logging.h"
#include "../HTTPCommon/HTTPRequestData.h"
#include "../HTTPCommon/HTTPResponseData.h"
#include "HTTPHandler.h"
#include "../HTTPCommon/HTTPException.h"
#include "../HTTPCommon/Utils.h"
#include "HTTPService.h"

#include "BasicService.h"

using namespace boost::algorithm;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::lexical_cast;
using boost::shared_ptr;
using boost::thread;
using std::list;
using std::map;
using std::multimap;
using std::string;
using std::vector;

using namespace HTTP;

void BasicService::registerHandler(boost::shared_ptr<HTTPHandler> hnd) {
    handlers.push_back(hnd);
}

void BasicService::deregisterHandler(boost::shared_ptr<HTTPHandler> hnd) {
    handlers.remove(hnd);
}

BasicService::BasicService(const std::string &ipaddr, const int port, const std::string &hostname)
    : signing_key(NULL),
      signing_key_length(0),
      srv_acceptor(service),
      srv_endpoint(ip::tcp::endpoint(ip::address_v4::from_string(ipaddr.c_str()), port)),
      m_hostname(hostname) { }

// Init() needs to be separate so that shared_from_this can give the shared_ptr to the workers in do_async_accept() without exploding everything
// It's weird, but it works. Read the docs on boost::weak_ptr and enable_shared_from_this for more information.
void BasicService::init() {
    // Initialize presalted hash state
    signing_key_length = 2048;
    signing_key = Platform::getRandomBytes(signing_key_length);

    // Bind and open acceptor socket
    srv_acceptor.open(srv_endpoint.protocol());
    srv_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    srv_acceptor.bind(srv_endpoint);
    srv_acceptor.listen();
    srv_endpoint = srv_acceptor.local_endpoint();
    FBLOG_INFO("HTTP:Service", "Started server on " << srv_endpoint.port());
    do_async_accept();

    // Start worker threadpool.
    for (std::size_t i = 0; i < threadpool_size; ++i) {
        shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&BasicService::_worker_thread_entry, this)));
        threadpool.push_back(thread);
    }
}

void BasicService::terminate() {
    service.stop();
    srv_acceptor.close();
    for (size_t i = 0; i < threadpool.size(); ++i) {
        threadpool[i]->interrupt();
    }
    threadpool.clear();
    deferred_shutdown_ref.reset();
}

BasicService::~BasicService() {
    terminate();
    delete[] signing_key;
}

void BasicService::_worker_thread_entry() {
    try {
        service.run();
    } catch (...) {
        // Don't let exceptions escape from the io_service, that'd result
        // in terminate() getting called. All bad.
    }
}

void BasicService::setDeferShutdown(bool val) {
    if (val) deferred_shutdown_ref = boost::dynamic_pointer_cast<BasicService>(shared_from_this());
    else deferred_shutdown_ref.reset();
}

FB::URI BasicService::getBaseUri() const {
    FB::URI res;
    res.protocol = "http";
    res.port = srv_endpoint.port();
    if (!m_hostname.empty()) {
        res.domain = m_hostname;
    } else {
        res.domain = srv_endpoint.address().to_string();
    }
    res.path = "/";
    return res;
}

std::string BasicService::tiger_hmac(const std::string& sign_str) const {
    const char opad = 0x5c, ipad = 0x36;

    size_t inner_size = signing_key_length + sign_str.size();
    boost::scoped_array<char> inner(new char[inner_size]);
    for (size_t i = 0; i < signing_key_length; ++i) {
        inner[i] = signing_key[i] ^ ipad;
    }
    memcpy(inner.get() + signing_key_length, sign_str.c_str(), sign_str.size());
    Tiger inner_hash(inner.get(), inner_size);

    size_t outer_size = signing_key_length + inner_hash.hash_size();
    boost::scoped_array<char> outer(new char[outer_size]);
    for (size_t i = 0; i < signing_key_length; ++i) {
        outer[i] = signing_key[i] ^ opad;
    }
    memcpy(outer.get() + signing_key_length, inner_hash.hash_data(), inner_hash.hash_size());
    Tiger outer_hash(outer.get(), outer_size);
    return std::string(outer_hash.hash_data(), outer_hash.hash_size());
}

void BasicService::sign_uri(FB::URI& in_uri) const {
    in_uri.query_data["_s"] = base64_encode(tiger_hmac(in_uri.path));
}

bool BasicService::check_uri_signature(const FB::URI& in_uri) {
    std::map<std::string, std::string>::const_iterator it = in_uri.query_data.find("_s");
    if (it == in_uri.query_data.end()) return false; // no sig

    return (base64_decode(it->second) == tiger_hmac(in_uri.path));
}

void BasicService::do_async_accept() {
    Session::ptr sp = new Session(service);
    srv_acceptor.async_accept(sp->socket(), boost::bind(&BasicService::handle_accept, this, _1, sp));
}

void BasicService::handle_accept(const boost::system::error_code& ec, BasicService::Session::ptr acc_sess) {
    if (!ec) {
        boost::shared_ptr<BasicService> service(boost::dynamic_pointer_cast<BasicService>(shared_from_this()));
        acc_sess->start(service);
        // TODO should we log accept errors?
    }

    do_async_accept();
}

