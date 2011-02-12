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
#ifndef H_HTTPREQUESTDATA
#define H_HTTPREQUESTDATA

#include <string>
#include <map>
#include <list>
#include "URI.h"
#include "HTTPDatablock.h"
#include "HTTPFileEntry.h"

namespace HTTP {

    class HTTPRequestData {
    public:
        friend class HTTPRequest;
        HTTPRequestData() {}
        HTTPRequestData(const FB::URI& in_uri, const std::string& in_method = std::string());
        ~HTTPRequestData();

        FB::URI uri;
        std::string method;
        std::multimap<std::string, std::string> headers;
        std::map<std::string, std::string> cookies;
        std::map<std::string, HTTPFileEntry> files;

        void addFile(const std::string& fieldname, const std::string& filename, const std::string& content_type, HTTPDatablock* contents);
    };

};

#endif // H_HTTPREQUESTDATA

