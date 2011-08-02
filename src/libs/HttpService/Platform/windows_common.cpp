/*
 *  windows_common.cpp
 *  fbplugin
 *
 */

#ifdef _WIN32

#include "windows_defs.h"
#include <windows.h>
#include <string>
#include <stdexcept>
using std::string;

void throw_GetLastError(const char* fnname);

string lpcwstr_to_utf8(const wchar_t* in_str) {
  int buffer_len = WideCharToMultiByte(CP_UTF8, 0, in_str, -1, NULL, 0, NULL, NULL);
  if (buffer_len == 0) throw_GetLastError("WideCharToMultiByte (lpcwstr_to_utf8)");
  string out_str;
  out_str.resize(buffer_len - 1);
  WideCharToMultiByte(CP_UTF8, 0, in_str, -1, const_cast<char*>(out_str.data()), (int)(out_str.size() + 1), NULL, NULL);
  return out_str;
}

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

