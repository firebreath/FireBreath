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

#include <stdlib.h>
#include <limits.h>
#include <boost/scoped_array.hpp>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <utf8.h>
#include <xlocale.h>
#include <wctype.h>
#endif

#include "utf8_tools.h"

namespace FB {

#ifdef _WIN32
    std::string lastError(const char* fnname) {
        DWORD errcode = GetLastError();
        char* buf = NULL;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL, errcode, 0, (LPSTR)&buf, 0, NULL);

        std::string err = std::string(fnname) + std::string(": ") + std::string(buf);
        LocalFree(buf);
        return err;
    }

    void throw_GetLastError(const char* fnname) {
        throw std::runtime_error(lastError(fnname).c_str());
    }
#endif

    std::string wstring_to_utf8(const std::wstring& src) {
#ifdef _WIN32
        int buffer_len = WideCharToMultiByte(CP_UTF8, 0, src.data(), src.size()+1, NULL, 0, NULL, NULL);
        if (buffer_len <= 0) throw_GetLastError("WideCharToMultiByte");
        std::string out_str;
        out_str.resize(buffer_len - 1);
        WideCharToMultiByte(CP_UTF8, 0, src.data(), src.size()+1, const_cast<char*>(out_str.data()), (int)(out_str.size() + 1), NULL, NULL);
        return out_str;
#else
        const size_t work_size = (src.size() * MB_LEN_MAX) + 1;
        boost::scoped_array<char> work(new char[work_size]);
        //size_t res = wcstombs(work.get(), src.data(), work_size);
        size_t res = wchar_to_utf8(src.data(), src.size() + 1, work.get(), work_size, UTF8_IGNORE_ERROR);
        if (res == 0) throw std::runtime_error("wstring_to_utf8: conversion error");
        return std::string(work.get());
#endif
    }


    std::wstring utf8_to_wstring(const std::string& src) {
#ifdef _WIN32
        if (src.empty()) return std::wstring();
        int buffer_len = MultiByteToWideChar(CP_UTF8, 0, src.data(), src.size()+1, NULL, 0);
        if (buffer_len <= 0) throw_GetLastError("MultiByteToWideChar");
        std::wstring res;
        res.resize(buffer_len - 1);
        MultiByteToWideChar(CP_UTF8, 0, src.data(), src.size()+1, const_cast<wchar_t*>(res.data()), (int)(res.size()+1));
        return res;
#else
        const size_t work_size = src.size()+1;
        boost::scoped_array<wchar_t> work(new wchar_t[work_size]);
        //size_t res = mbstowcs(work.get(), src.data(), work_size);
        size_t res = utf8_to_wchar(src.data(), src.size()+1, work.get(), work_size, UTF8_IGNORE_ERROR);
        if (res == ((size_t)-1)) throw std::runtime_error("utf8_to_wstring: conversion error");
        return std::wstring(work.get());
#endif
    }


    std::wstring wstring_tolower(const std::wstring& src) {
      if (src.empty()) return std::wstring();
      
      std::wstring dest;
      dest.resize(src.size());
      
#ifdef _WIN32
      if (LCMapString(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, src.data(), src.size(), const_cast<wchar_t*>(dest.data()), dest.size()) == 0) throw_GetLastError("wstring_tolower");
      
#else
      for (size_t i = 0; i < src.size(); ++i) {
        dest[i] = towlower(src[i]);
      }
#endif

      return dest;
    }

};

