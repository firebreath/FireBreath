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
#ifndef H_HTTPHANDLER
#define H_HTTPHANDLER

#include "../HTTPCommon/HTTPDatablock.h"
#include "../HTTPCommon/HTTPResponseData.h"
#include "../HTTPCommon/HTTPRequestData.h"

namespace HTTP {
    class HTTPHandler {
    public:
        virtual ~HTTPHandler() {}

        // Flag whether or not the handler requires URI verification.
        // If this is true, request URIs without a verifiable signature will
        // not be passed to handleRequest.
        virtual bool requiresVerifiedURI() const { return true; }
        // If the handler can't respond to this request, it returns NULL; otherwise, a new HTTPResponseData.
        // Actual errors throw HTTPException.
        virtual HTTPResponseData* handleRequest(const HTTPRequestData& req) = 0;
    };
};

#endif // H_HTTPHANDLER

