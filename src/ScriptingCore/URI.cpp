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

#ifdef _WIN32
#include "win_targetver.h"
#endif
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/logic/tribool.hpp>
#include <vector>
#include <sstream>
#include <boost/asio.hpp>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "URI.h"

using namespace boost::algorithm;
using namespace boost::logic;
using std::string;
using std::vector;
using FB::URI;

URI::StringStringMap URI::m_lhMap;

std::string URI::url_encode(const std::string& in) {
    std::stringstream res;
    for (size_t i = 0; i < in.size(); ++i) {
        char c = in[i];
        if (c > 0 && (isalnum(c) ||
            c == '$' || c == '-' || c == '_' || c == '.' || c == '!' ||
            c == '*' || c == '\''|| c == '(' || c == ')' || c == ',' || c == '/')) res << c;
        else {
            char buf[4];
            sprintf(buf, "%%%.2x", c & 0xff);
            res << buf;
        }
    }
    return res.str();
}

std::string URI::url_decode(const std::string& in) {
    std::stringstream res;
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%' && (i + 2) < in.size() && isxdigit(in[i+1]) && isxdigit(in[i+2])) {
            char buf[3];
            ++i;
            buf[0] = in[i++]; buf[1] = in[i]; buf[2] = '\0';
            res << ((char)strtol(buf, NULL, 16));
        } else res << in[i];
    }
    return res.str();
}

std::string URI::toString(bool include_host_part) const {
    std::stringstream res;
    if (include_host_part) {
        res << protocol << string("://");
        if (!login.empty()) res << login << "@";
        res << domain;
        if (port) res << ":" << boost::lexical_cast<string>(port);
    }
    res << url_encode(path);
    if (!query_data.empty()) {
        char separator = '?';
        for (std::map<std::string, std::string>::const_iterator it = query_data.begin(); it != query_data.end(); ++it) {
            res << separator;
            separator = '&';
            res << url_encode(it->first);
            res << '=';
            res << url_encode(it->second);
        }
    }
    if (!fragment.empty())
        res << "#" << fragment;
    return res.str();
}

URI URI::fromString(const std::string& in_str) {
    return URI(in_str);
}

URI::URI(const std::string& in_str) : port(0) {
    string w = in_str;

    size_t l = w.find("://");
    if (l != std::string::npos) {
        protocol = w.substr(0, l);
        std::transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);
        w = w.substr(l + 3);
    }
    // validate protocol -- should only contain [a-z0-9]
    for (l = 0; l < protocol.size(); ++l) {
        if (!isalnum(protocol[l])) throw std::runtime_error("URI: invalid characters in protocol part");
    }

    if (protocol != "file") { // file has neither a domain nor a port
        l = w.find_first_of("/\\");
        // chomp at the '/' (if it exists) so parsing the login/domain/port is easier
        string domain_str;
        if (l == std::string::npos) {
            domain_str = w;
            w = "/";
        } else {
            domain_str = w.substr(0, l);
            w = w.substr(l);
        }

        // check for login info
        l = domain_str.find("@");
        if (l != std::string::npos) {
            login = domain_str.substr(0, l);
            domain_str = domain_str.substr(l + 1);
        }

        // split port, if it exists
        size_t p = domain_str.find(":");
        if (p != std::string::npos && p < l) {
            domain = domain_str.substr(0, p);
            string port_str = domain_str.substr(p + 1);
            port = boost::lexical_cast<int>(port_str);
        } else {
            domain = domain_str;
        }
        // domains are case insensitive; transform to lower case for convenience.
        std::transform(domain.begin(), domain.end(), domain.begin(), ::tolower);
    }

    l = w.find('#');
    if (l != std::string::npos) {
        fragment = w.substr(l + 1);
        w = w.substr(0, l);
    }
    l = w.find('?');
    if (l != std::string::npos) {
        parse_query_data(w.substr(l + 1));
        w = w.substr(0, l);
    }
    path = url_decode(w);
}

bool URI::operator==(const URI& right) const {
    return ( (protocol == right.protocol) &&
        (login == right.login) &&
        (domain == right.domain) &&
        (port == right.port) &&
        (path == right.path) &&
        (fragment == right.fragment) &&
        (query_data == right.query_data)
        );
}

void URI::appendPathComponent(const std::string& pc) {
    if (! pc.size()) return;
    // make sure we have exactly one '/' between the old path and new path component[s]
    if (path.size() && path[path.size() - 1] == '/') path.resize(path.size() - 1);
    if (pc[0] != '/') path.push_back('/');
    path += pc;
}

std::string URI::filename() const {
    if (path.empty() || path[path.size()-1] == '/') return string();
    size_t loc = path.rfind("/");
    if (loc == std::string::npos) return path;
    return path.substr(loc+1);
}

void URI::resetValidLocalhost()
{
    m_lhMap.clear();
}
void URI::registerValidLocalhost(std::string domain, std::string ip)
{
    if (ip.empty()) {
        ip = boost::asio::ip::address_v4::loopback().to_string();
    }
    m_lhMap[domain] = ip; 
}

bool URI::isLocalhost() const {
    // To avoid a security breach by DNS poisioning, we make sure that the allowed
    // domain (either localhost or registered with registerValidLocalhost) actually
    // is pointing at localhost / the correct IP.
    StringStringMap tmp;
    StringStringMap::const_iterator fnd(m_lhMap.find(domain));
    if (fnd != m_lhMap.end()) {
        if (fnd->first == boost::asio::ip::address_v4::loopback().to_string()) {
            return true;
        }
        static boost::tribool lastResult = indeterminate;
        if (lastResult != indeterminate) return lastResult;

        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        // The resolver wants a service name, that's what the "80" is.
        // The numeric_service flag tells it not to do anything with the service name anyway.
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), domain, "80", boost::asio::ip::resolver_query_base::numeric_service);
        boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
        if (it == boost::asio::ip::tcp::resolver::iterator()) return false;
        boost::asio::ip::tcp::endpoint ep = it->endpoint();

        lastResult = (bool) (ep.address() == boost::asio::ip::address_v4::loopback());
        return lastResult;
    }
    return false;
}

void URI::parse_query_data(const std::string& in_str) {
    std::vector<string> parts;
    split(parts, in_str, is_any_of("&"));
    for (std::vector<string>::iterator it = parts.begin(); it != parts.end(); ++it) {
        std::vector<string> kvp;
        split(kvp, *it, is_any_of("="));
        if (kvp.empty()) continue;
        else if (kvp.size() == 1) query_data[url_decode(kvp[0])] = string();
        else query_data[url_decode(kvp[0])] = url_decode(kvp[1]);
    }
}

