/*
 *  mac_common.h
 *  fbplugin
 *
 */

#pragma once
#ifdef XP_MACOSX
#include <CoreFoundation/CoreFoundation.h>
#include <string>

std::string conv_cfstring(CFStringRef s);
int cfnumber_to_int(CFNumberRef n);
#endif

