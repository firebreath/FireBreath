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
#ifndef H_HTTP_HTTPDATABLOCK
#define H_HTTP_HTTPDATABLOCK
#include <string>

namespace HTTP {
    class HTTPDatablock {
    public:
        virtual ~HTTPDatablock() {}
        virtual size_t size() const = 0;
        virtual const char* data() const = 0;
        virtual void resolve() const {}
    };

    class HTTPStringDatablock : public HTTPDatablock {
    public:
        HTTPStringDatablock(const std::string& _str) : str(_str) {}
        HTTPStringDatablock(const char* _data, size_t _len) : str(std::string(_data, _len)) {}
        HTTPStringDatablock() {}
        virtual ~HTTPStringDatablock() {}
        virtual size_t size() const { return str.size(); }
        virtual const char* data() const { return str.data(); }
    protected:
        std::string str;
    };
};

#endif

