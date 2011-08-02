/*
 * windows_defs.h
 * fbplugin
 *
 */

#pragma once
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4251)
#pragma warning(disable:4800)
#pragma warning(disable:4996)
#pragma warning(disable:4068) // unknown #pragma

//#include <basetsd.h> // for SSIZE_T
#include <string>

#define ssize_t SSIZE_T
#define snprintf _snprintf
#define bzero(ptr, size) memset(ptr, 0, size);

#define MAX_UNICODE_PATH 32768 // see http://msdn.microsoft.com/en-us/library/aa365247(VS.85).aspx -- Maximum Path Length section.

// these are defined in windows_common.cpp
namespace FBPlugin {
    std::string lastError(const char* fnname);
    void throw_GetLastError(const char* fnname);
    std::string lpcwstr_to_utf8(const wchar_t* in_str);
}
#endif

