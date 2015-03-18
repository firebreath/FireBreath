/**********************************************************\
Original Author: Dan Weatherford
Imported with permission by: Richard Bateman (taxilian)

Imported:   Aug 7, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Dan Weatherford, Facebook inc
\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include <limits.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <locale>
#include <codecvt>

#include "utf8_tools.h"

std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

namespace FB {

    std::string wstring_to_utf8(const std::wstring& src) {
        return utf8_conv.to_bytes(src);
    }


    std::wstring utf8_to_wstring(std::string src) {
        return utf8_conv.from_bytes(src);
    }


    std::wstring wstring_tolower(const std::wstring& src) {
        return boost::algorithm::to_upper_copy(src);
    }

};

