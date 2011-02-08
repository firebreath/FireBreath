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
#ifndef H_HTTPRESPONSEDATA
#define H_HTTPRESPONSEDATA

#include <string>
#include <map>
#include <list>
#include "URI.h"

#include "HTTPDatablock.h"

namespace HTTP {
    class HTTPResponseData {
    public:
        HTTPResponseData() : code(200) {}
        HTTPResponseData(HTTPDatablock* b) { data.push_back(b); }
        ~HTTPResponseData();

        void addDatablock(HTTPDatablock* b) { data.push_back(b); }
        HTTPDatablock* coalesceBlocks();

        void setNoncacheable(); // Add all of the usual cache-control headers to prevent the browser from caching the response.

        unsigned int code;
        std::string code_detail; // The full first response line. "HTTP/1.1 200 OK", etc
        std::multimap<std::string, std::string> headers;
        std::map<std::string, std::string> cookies;
        std::list<HTTPDatablock*> data;
    };
};

#endif // H_HTTPRESPONSEDATA

