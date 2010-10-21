/*
 *  mac_common.h
 *  fbplugin
 *
 */

#pragma once
#include <CoreFoundation/CoreFoundation.h>
#include <string>

std::string conv_cfstring(CFStringRef s);
int cfnumber_to_int(CFNumberRef n);
