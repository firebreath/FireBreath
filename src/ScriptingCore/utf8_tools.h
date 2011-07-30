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
#include "../3rdParty/utf8/utf8.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn std::string wstring_to_utf8(const std::wstring& src)
    ///
    /// @brief  Accepts a std::wstring and returns a UTF8-encoded std::string
    ///
    /// @param  src Source string
    ///
    /// @return UTF8-encoded std::string
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string wstring_to_utf8(const std::wstring& src);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn std::wstring utf8_to_wstring(const std::string& src)
    ///
    /// @brief  Accepts a UTF8-encoded std::string and returns a std::wstring
    ///
    /// @param  src Source string
    ///
    /// @return std::wstring representation
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::wstring utf8_to_wstring(const std::string& src);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn std::wstring wstring_tolower(const std::wstring& src)
    ///
    /// @brief  Converts a std::wstring to lowercase
    ///
    /// @param  src Source string
    ///
    /// @return lowercase version of the source string
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::wstring wstring_tolower(const std::wstring& src);

};

#endif // H_FB_UTF8

