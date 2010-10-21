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
#ifndef H_FB_URI
#define H_FB_URI
#include <string>
#include <map>
#include <set>
#include "APITypes.h"

namespace FB {
    class URI {
    public:
        static std::string url_decode(const std::string& in);
        static std::string url_encode(const std::string& in);

    public:
        URI() : port(0) {}
        URI(const std::string& str_uri);
        bool operator==(const URI& right) const;

        std::string toString(bool include_domain_part = true) const;
        static URI fromString(const std::string& in_str);

        std::string protocol;
        std::string login;
        std::string domain;
        uint16_t port;
        std::string path;
        std::map<std::string, std::string> query_data;
        std::string fragment;

    protected:
        typedef std::map<std::string, std::string> StringStringMap;
        static StringStringMap m_lhMap;

    public:
        static void resetValidLocalhost();
        static void registerValidLocalhost(std::string url, std::string ip = "");

        void appendPathComponent(const std::string& pc);
        std::string filename() const; // last path component; if the URI ends in a trailing slash, returns the empty string.
        bool isLocalhost() const;

        void parse_query_data(const std::string& in_str);
    };
};

#endif // H_FB_URI