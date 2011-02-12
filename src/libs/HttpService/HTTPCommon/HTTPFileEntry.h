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
#ifndef H_HTTPCOMMON
#define H_HTTPCOMMON

#include <string>
#include <map>
#include "HTTPDatablock.h"
#include "URI.h"

namespace HTTP {
    struct HTTPFileEntry {
      std::string filename;
      std::string content_type;
      HTTPDatablock* contents;
    };
};

#endif // H_HTTPCOMMON

