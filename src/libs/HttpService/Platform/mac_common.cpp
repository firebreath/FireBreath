/*
 *  mac_common.cpp
 *  fbplugin
 *
 */

#include "mac_common.h"

#ifdef XP_MACOSX

std::string conv_cfstring(CFStringRef s) {
  size_t l = (CFStringGetLength(s) * 2) + 1;
  char* q = new char[l];
  q[0] = '\0';
  CFStringGetCString(s, q, l, kCFStringEncodingUTF8);
  std::string res(q);
  delete[] q;
  return res;
}

int cfnumber_to_int(CFNumberRef n) {
  if (! n) return 0;
  
  int nn;
  CFNumberGetValue(n, kCFNumberIntType, &nn);
  return nn;
}

#endif

