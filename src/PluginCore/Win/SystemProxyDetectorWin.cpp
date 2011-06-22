
#include "Win/SystemProxyDetectorWin.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH

#include <boost/algorithm/string.hpp>
#include <wininet.h>
#include <vector>

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using std::string;
using std::vector;
using std::map;

FB::SystemProxyDetector* FB::SystemProxyDetector::get()
{
    static boost::scoped_ptr<FB::SystemProxyDetector> _inst(new FB::SystemProxyDetectorWin());
    return _inst.get();
}

namespace {

    string lastError(const char* fnname) {
        DWORD errcode = GetLastError();
        char* buf = NULL;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, errcode, 0, (LPSTR)&buf, 0, NULL);

        std::stringstream stream;
        stream << fnname << ": " << buf;
        LocalFree(buf);
        return stream.str();
    }

    void throw_GetLastError(const char* fnname) {
        throw std::runtime_error(lastError(fnname).c_str());
    }

}

bool FB::SystemProxyDetectorWin::detectProxy( map<string, string>& proxyMap, const string& URL )
{
    proxyMap.clear();
    try {
        // The buffer that InternetQueryOption wants contains all of the strings after the struct in one
        // alloc, that's why this is variable size instead of just sizeof(INTERNET_PROXY_INFO)
        DWORD ipiSize = 0;
        InternetQueryOption(NULL, INTERNET_OPTION_PROXY, NULL, &ipiSize);
        boost::scoped_array<char> ipiBuf(new char[ipiSize]);
        INTERNET_PROXY_INFO* ipi = reinterpret_cast<INTERNET_PROXY_INFO*>(ipiBuf.get());

        if (! InternetQueryOption(NULL, INTERNET_OPTION_PROXY, ipiBuf.get(), &ipiSize)) throw_GetLastError("InternetQueryOption(INTERNET_OPTION_PROXY)");

        if (ipi->dwAccessType == INTERNET_OPEN_TYPE_PROXY) {
            FB::URI uri(FB::URI::fromString(URL));
            // Proxy config looks like:
            // socks=socks_proxy_server:1080 https=https_proxy_server:4443 ftp=ftp_proxy_server:2222 http=http_proxy_server:3128
            // any of the key-value pairs may be omitted
            // or, if it's just a single proxy server (which is HTTP):
            // http_proxy_server:3128

            string proxyConfig(reinterpret_cast<const char*>(ipi->lpszProxy));

            vector<string> proxy_parts;
            split(proxy_parts, proxyConfig, is_any_of(" "));
            for (size_t i = 0; i < proxy_parts.size(); ++i) {
                vector<string> kvp;
                split(kvp, proxy_parts[i], is_any_of("="));
                if (kvp.empty()) continue;
                if (kvp.size() == 1) {
                    // untyped proxy entry. just put http on the front of the array, then fall through to the other case
                    kvp.insert(kvp.begin(), string("http"));
                }

                // this is a typed proxy entry. kvp[0] should be one of {socks,https,ftp,http}
                vector<string> host_parts;
                split(host_parts, kvp[1], is_any_of(":"));
                if (host_parts.size() < 2) continue; // what?

                if (kvp[0] == uri.protocol || (proxyMap.find("type") == proxyMap.end() && kvp[0] == "socks")) {
                    // If we find a match for this exact protocol *or* if there is a socks proxy
                    proxyMap["port"] = host_parts[1];
                    proxyMap["hostname"] = host_parts[0];
                    proxyMap["type"] = kvp[0];
                }
            }
        }
        return proxyMap.find("type") != proxyMap.end();
    } catch (const std::exception& e) {
        // TODO: This isn't a very good place to log this.
        FBLOG_WARN("SystemProxyDetector", "Could not detect system proxy: " << e.what());
        return false;
    }
}