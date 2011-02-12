/*
 *  base64.cpp
 *  fbplugin
 *
 */

#include "base64.h"
#include <string.h>

static const char* base64_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

std::string base64_encode(const std::string& indata) {
  std::string outdata;
  outdata.reserve(((indata.size() * 8) / 6) + 2);
  std::string::size_type remaining = indata.size();
  const char* dp = indata.data();

  while (remaining >= 3) {
    outdata.push_back(base64_charset[(dp[0] & 0xfc) >> 2]);
    outdata.push_back(base64_charset[((dp[0] & 0x03) << 4) | ((dp[1] & 0xf0) >> 4)]); 
    outdata.push_back(base64_charset[((dp[1] & 0x0f) << 2) | ((dp[2] & 0xc0) >> 6)]);
    outdata.push_back(base64_charset[(dp[2] & 0x3f)]);
    remaining -= 3; dp += 3;
  }
  
  if (remaining == 2) {
    outdata.push_back(base64_charset[(dp[0] & 0xfc) >> 2]);
    outdata.push_back(base64_charset[((dp[0] & 0x03) << 4) | ((dp[1] & 0xf0) >> 4)]); 
    outdata.push_back(base64_charset[((dp[1] & 0x0f) << 2)]);
    outdata.push_back(base64_charset[64]);
  } else if (remaining == 1) {
    outdata.push_back(base64_charset[(dp[0] & 0xfc) >> 2]);
    outdata.push_back(base64_charset[((dp[0] & 0x03) << 4)]); 
    outdata.push_back(base64_charset[64]);
    outdata.push_back(base64_charset[64]);
  }

  return outdata;
}

std::string base64_decode(const std::string& indata) {
  std::string outdata;
  outdata.reserve((indata.size() * 6) / 8);
  static char* xtbl = NULL;
  if (! xtbl) { 
    // Build translation table from base64_charset string (once)
    xtbl = new char[256];
    memset(xtbl, 0, 256);
    for (char s = 0; s < 64; ++s) {
      xtbl[base64_charset[s]] = s; 
    }
    xtbl[base64_charset[64]] = 0; // padding character
  }

  std::string::size_type remaining = indata.size();
  const char* p = indata.data();
  while (remaining >= 4) {
    char xp[4];
    for (size_t s = 0; s < 4; ++s) xp[s] = xtbl[p[s]]; 
    outdata.push_back((xp[0] << 2) | ((xp[1] & 0x30) >> 4));
    outdata.push_back(((xp[1] & 0x0f) << 4) | ((xp[2] & 0x3c) >> 2));
    outdata.push_back(((xp[2] & 0x03) << 6) | xp[3]);
    remaining -= 4;
    if (remaining == 0) {
      if (p[3] == base64_charset[64]) outdata.resize(outdata.size() - 1);
      if (p[2] == base64_charset[64]) outdata.resize(outdata.size() - 1);
      break;
    }
    p += 4;
  }
  if (remaining) { // compatibility for old, broken padding
    while (*(p++) == base64_charset[64]) outdata.resize(outdata.size() - 1); // pop_back
  }
  return outdata;
}

