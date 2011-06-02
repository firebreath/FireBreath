
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCDynamicStore.h>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <vector>

#include "URI.h"

#include "Mac/SystemProxyDetectorMac.h"

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using std::string;
using std::vector;
using std::map;

std::string conv_cfstring(CFStringRef s) {
    size_t l = (CFStringGetLength(s) * 2) + 1;
    boost::scoped_array<char> q(new char[l]);
    q[0] = '\0';
    CFStringGetCString(s, q.get(), l, kCFStringEncodingUTF8);
    std::string res(q.get());
    return res;
}

int cfnumber_to_int(CFNumberRef n) {
    if (! n) return 0;
    
    int nn;
    CFNumberGetValue(n, kCFNumberIntType, &nn);
    return nn;
}

FB::SystemProxyDetector* FB::SystemProxyDetector::get()
{
    static boost::scoped_ptr<FB::SystemProxyDetector> _inst(new FB::SystemProxyDetectorMac());
    return _inst.get();
}

bool FB::SystemProxyDetectorMac::detectProxy( map<string, string>& proxyMap, const string& URL )
{
    CFDictionaryRef proxySettings(SCDynamicStoreCopyProxies(NULL));
    if (! proxySettings) {
        CFRelease(proxySettings);
        return false;
    }
    FB::URI uri(FB::URI::fromString(URL));
    /*
     * -- relevant dictionary keys
     *   kSCPropNetProxiesHTTPEnable                        "HTTPEnable"                   CFNumber (0 or 1)
     *   kSCPropNetProxiesHTTPPort                          "HTTPPort"                     CFNumber
     *   kSCPropNetProxiesHTTPProxy                         "HTTPProxy"                    CFString
     *   kSCPropNetProxiesSOCKSEnable                       "SOCKSEnable"                  CFNumber (0 or 1)
     *   kSCPropNetProxiesSOCKSPort                         "SOCKSPort"                    CFNumber
     *   kSCPropNetProxiesSOCKSProxy                        "SOCKSProxy"                   CFString
     */
    
    if (uri.protocol == "http" && cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPEnable))) {
        // HTTP proxy?
        proxyMap["type"] = uri.protocol;
        proxyMap["hostname"] = conv_cfstring((CFStringRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPProxy));
        int port = cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPPort));
        proxyMap["port"] = boost::lexical_cast<std::string>(port);
    } else if (uri.protocol == "https" && cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPSEnable))) {
        // HTTPS proxy?
        proxyMap["type"] = uri.protocol;
        proxyMap["hostname"] = conv_cfstring((CFStringRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPSProxy));
        int port = cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPSPort));
        proxyMap["port"] = boost::lexical_cast<std::string>(port);
    } else if (cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSEnable))) {
        // SOCKS proxy on fallback
        proxyMap["type"] = "socks";
        proxyMap["hostname"] = conv_cfstring((CFStringRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSProxy));
        int port = cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSPort));
        proxyMap["port"] = boost::lexical_cast<std::string>(port);
    }
    
    CFRelease(proxySettings);
    if (proxyMap.find("type") != proxyMap.end())
        return true;
    else
        return false;
}
