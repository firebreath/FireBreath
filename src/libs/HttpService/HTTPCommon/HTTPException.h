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
#ifndef H_HTTPEXCEPTION
#define H_HTTPEXCEPTION

#include <string>
#include <map>
#include "URI.h"

namespace HTTP {

    class HTTPException : public std::exception {
    public:
        HTTPException(short _response_code, const char* _msg);
        virtual ~HTTPException() throw() { free(msg); }
        virtual const char* what() const throw() { return msg; }
        std::string getResponseHeader() const;
    protected:
        short response_code;
        char* msg;
    };

};

#endif // H_HTTPEXCEPTION

