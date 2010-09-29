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

#pragma once
#ifndef H_FB_UTF8
#define H_FB_UTF8

#include <string>

namespace FB {

#ifdef _WIN32
    std::string lastError(const char* fnname);
    void throw_GetLastError(const char* fnname);
#endif

    std::string wstring_to_utf8(const std::wstring& src);
    std::wstring utf8_to_wstring(const std::string& src);

    std::wstring wstring_tolower(const std::wstring& src);

};

#endif // H_FB_UTF8

