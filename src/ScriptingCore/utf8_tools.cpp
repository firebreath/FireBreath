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

#ifdef _WIN32
#include <windows.h>
#else
#include "../3rdParty/utf8/utf8.h"
#include <xlocale.h>
#include <wctype.h>
#endif
#include <stdexcept>

#include <boost/scoped_array.hpp>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include <limits.h>
#include <boost/algorithm/string/case_conv.hpp>

#include "utf8_tools.h"

namespace FB {

    std::string wstring_to_utf8(const std::wstring& src) {
        std::string out_str;
#ifdef _WIN32
        utf8::utf16to8(src.begin(), src.end(), std::back_inserter(out_str));
#else
        utf8::utf32to8(src.begin(), src.end(), std::back_inserter(out_str));
#endif
        return out_str;
    }


    std::wstring utf8_to_wstring(const std::string& src) {
        std::wstring out_str;
#ifdef _WIN32
        utf8::utf8to16(src.begin(), src.end(), std::back_inserter(out_str));
#else
        utf8::utf8to32(src.begin(), src.end(), std::back_inserter(out_str));
#endif
        return out_str;
    }


    std::wstring wstring_tolower(const std::wstring& src) {
        return boost::algorithm::to_upper_copy(src);
    }

};

