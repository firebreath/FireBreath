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

#include "HTTPException.h"

using HTTP::HTTPException;

HTTPException::HTTPException(short _response_code, const char* _msg) : response_code(_response_code) {
  msg = strdup(_msg);
}
std::string HTTPException::getResponseHeader() const {
    switch (response_code) {
#define RC(code, msg) case code: return ("HTTP/1.1 " #code " " msg); break;
      RC(400, "Bad Request")
      RC(401, "Unauthorized")
      RC(403, "Forbidden")
      RC(404, "Not Found")
      RC(405, "Method Not Allowed")
      RC(415, "Unsupported Media Type")
        default:
      RC(500, "Internal Server Error")
      RC(501, "Not Implemented")
#undef RC
    };
}

