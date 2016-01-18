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
#ifdef __linux
#include "../3rdParty/utf8/utf8.h"
#else 
#include <codecvt>
#endif

#include "utf8_tools.h"

#ifndef __linux 
std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
#endif

namespace FB {

    std::string wstring_to_utf8(const std::wstring& src) { 
#ifdef __linux
        std::string out_str;
        std::wstring in_str;
        utf8::replace_invalid(src.begin(), src.end(), std::back_inserter(in_str));
        utf8::utf32to8(in_str.begin(), in_str.end(), std::back_inserter(out_str));
        return out_str;
#else
        return utf8_conv.to_bytes(src);
#endif
    }
    
    std::wstring utf8_to_wstring(std::string src) {
#ifdef __linux
        std::wstring out_str;
        std::string in_str;
        utf8::replace_invalid(src.begin(), src.end(), std::back_inserter(in_str));
        utf8::utf8to32(in_str.begin(), in_str.end(), std::back_inserter(out_str));
        return out_str;
#else
        return utf8_conv.from_bytes(src);
#endif
    }


    std::wstring wstring_tolower(const std::wstring& src) {
        return boost::algorithm::to_upper_copy(src);
    }

};

