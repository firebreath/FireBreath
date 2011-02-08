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

#include "URI.h"
#include <boost/algorithm/string.hpp>
#include "HTTPException.h"

#include "Utils.h"

using namespace boost::algorithm;
using std::string;
using std::map;
using std::multimap;

std::multimap<std::string, std::string> HTTP::parse_http_headers(
    std::vector<std::string>::const_iterator begin,
    std::vector<std::string>::const_iterator end )
{
    multimap<string, string> res;
    for (std::vector<string>::const_iterator it = begin; it != end; ++it) {
        string line = trim_copy(*it);
        if (line.empty()) continue;
        size_t loc = line.find(':');
        if (loc == std::string::npos) {
            throw HTTP::HTTPException(400, "Malformed header");
        }
        res.insert(std::make_pair(trim_copy(line.substr(0, loc)), trim_copy(line.substr(loc + 1))));
    }
    return res;
}

std::string HTTP::build_cookie_header( const std::map<std::string, std::string>& cookies )
{
    std::vector<string> cookie_crumbs;
    cookie_crumbs.reserve(cookies.size());
    for (map<string, string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it) {
        cookie_crumbs.push_back(FB::URI::url_encode(it->first) + '=' + FB::URI::url_encode(it->second));
    }

    return join(cookie_crumbs, ";");
}

