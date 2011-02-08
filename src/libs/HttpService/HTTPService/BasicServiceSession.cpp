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

#include "BasicService.h"
#include <boost/algorithm/string.hpp>
#include "../HTTPCommon/HTTPException.h"
#include "../HTTPCommon/Utils.h"
#include "logging.h"

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

BasicService::Session::Session(boost::asio::io_service& svc) : sock(svc) {

}

BasicService::Session::~Session() {

}

void BasicService::Session::start(const boost::shared_ptr<BasicService>& _parent_svc) {
    parent_svc = _parent_svc;
    wait_for_header();
}

void BasicService::Session::wait_for_header() {
    async_read_until(sock, data, "\r\n\r\n", boost::bind(&Session::handle_request, BasicService::Session::ptr(this), _1));  
}


void BasicService::Session::handle_request(boost::system::error_code ec) {
    if (ec) {
        FBLOG_WARN("Http:BasicService", "handle_request error message: " << ec.message());
        return;
    }

    std::vector<string> header_lines;
    std::istream data_stream(&data);
    while (!data_stream.eof()) {
        string tmp;
        std::getline(data_stream, tmp);
        trim(tmp);
        if (tmp.size() && tmp.at(tmp.size() - 1) == '\r') tmp.resize(tmp.size() - 1);
        if (tmp.empty()) break;
        header_lines.push_back(tmp);
    }

    HTTPRequestData req_data;
    HTTPResponseData* resp = NULL;

    // First line is like: GET /honk/test?asdf=1234 HTTP/1.1
    // Following lines are "Property-Name: Property-Value"
    // (your basic http stuff)
    // The path is entity-encoded; "%20" = character 0x20 (which is a space), for example
    try {
        std::vector<string> req_parts;
        split(req_parts, header_lines[0], is_any_of(" "));
        if (req_parts.size() != 3) throw HTTPException(400, "Malformed Request-Line");
        req_data.method = req_parts[0];
        req_data.uri = FB::URI::fromString(req_parts[1]);

        // Split the rest of the header lines into the request data
        req_data.headers = parse_http_headers(++header_lines.begin(), header_lines.end());

        if (req_data.uri.path == "/shutdown") {
            FBLOG_INFO("Http:BasicServiceSession", "Received shutdown request");
            parent_svc->setDeferShutdown(false);
            resp = new HTTPResponseData;
            resp->code = 200;
            // No response payload necessary.

        }   else if (parent_svc->handlers.empty()) {
            resp = new HTTPResponseData;
            resp->headers.insert(std::make_pair("Connection", "Close"));
            resp->headers.insert(std::make_pair("Content-Type", "text/plain"));

            std::string response_str = string("No handlers are available. Dumping request details...\r\n");
            response_str += req_data.uri.toString(false) + string("\r\nParameters:\r\n\r\n");
            for (std::map<std::string, std::string>::const_iterator it = req_data.uri.query_data.begin(); it != req_data.uri.query_data.end(); ++it) {
                response_str += it->first + string(": ") + it->second + string("\r\n");
            }
            response_str += string("\r\nHeaders:\r\n\r\n");
            for (std::multimap<std::string, std::string>::const_iterator it = req_data.headers.begin(); it != req_data.headers.end(); ++it) {
                response_str += it->first + string(": ") + it->second + string("\r\n");
            }
            resp->addDatablock(new HTTPStringDatablock(response_str));
        } else {
            bool verified = parent_svc->check_uri_signature(req_data.uri);
            for (std::list<boost::shared_ptr<HTTPHandler> >::const_iterator it = parent_svc->handlers.begin(); it != parent_svc->handlers.end(); ++it) {
                if ((!verified) && ((*it)->requiresVerifiedURI())) continue;
                if ((resp = (*it)->handleRequest(req_data))) break;
            }
            if ((!resp) && (!verified)) throw HTTPException(500, "No registered handlers responded to this request, possibly because of a missing or invalid signature.");
        }

        if (!resp) throw HTTPException(500, "No registered handlers responded to this request.");

        // Response obtained. Stringify headers and add them to the head of the block list
        {
            // Compute content-length first
            size_t content_length = 0;
            for (std::list<HTTPDatablock*>::const_iterator it = resp->data.begin(); it != resp->data.end(); ++it) {
                content_length += (*it)->size();
            }

            resp->headers.erase("Content-Length");
            resp->headers.insert(std::make_pair("Content-Length", lexical_cast<string>(content_length)));

            resp->headers.erase("Connection");
            resp->headers.insert(std::make_pair("Connection", "close"));

            std::ostringstream header_os;
            header_os << "HTTP/1.1 " << resp->code << " OK\r\n";
            for (std::multimap<std::string, std::string>::const_iterator it = resp->headers.begin(); it != resp->headers.end(); ++it) {
                header_os << it->first << ": " << it->second << "\r\n";
            }
            for (std::map<std::string, std::string>::const_iterator it = resp->cookies.begin(); it != resp->cookies.end(); ++it) {
            }
            header_os << "\r\n";
            resp->data.push_front(new HTTPStringDatablock(header_os.str()));
        }

    } catch (const HTTPException& e) {
        if (resp) delete resp;
        FBLOG_INFO("Http:BasicServiceSession", "HTTP exception: " << e.what());
        resp = new HTTPResponseData(new HTTPStringDatablock(e.getResponseHeader() + string("\r\nContent-Type: text/plain\r\n\r\n") + e.what()));
    } catch (const std::exception& e) {
        if (resp) delete resp;
        FBLOG_INFO("Http:BasicServiceSession", "std::exception: " << e.what());
        resp = new HTTPResponseData(new HTTPStringDatablock(string("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n") + e.what()));
    }
    // And write the response datablock list.
    async_write(sock, buffer((*resp->data.begin())->data(), (*resp->data.begin())->size()), boost::bind(&Session::handle_response_datablock_complete, BasicService::Session::ptr(this), _1, resp));
}

void BasicService::Session::handle_response_datablock_complete(boost::system::error_code ec, HTTPResponseData* resp) {
    if (ec) {
        delete resp;
        sock.close();
        return;
    }
    delete resp->data.front();
    resp->data.pop_front();
    if (resp->data.empty()) {
        delete resp;
        sock.close();
        return;
    }
    async_write(sock, buffer((*resp->data.begin())->data(), (*resp->data.begin())->size()), boost::bind(&Session::handle_response_datablock_complete, BasicService::Session::ptr(this), _1, resp));
}

