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
#ifndef HTTP_Utils_h__
#define HTTP_Utils_h__

#include <string>
#include <map>
#include <vector>

namespace HTTP {
    std::multimap<std::string, std::string> parse_http_headers(
        std::vector<std::string>::const_iterator begin,
        std::vector<std::string>::const_iterator end);

    std::string build_cookie_header(const std::map<std::string, std::string>& cookies);
}
#endif // HTTP_Utils_h__

