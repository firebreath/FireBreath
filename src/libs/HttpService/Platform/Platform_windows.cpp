/*
 *  Platform_windows.cpp
 *  fbplugin
 *
 */

#include "windows_defs.h"

#include <stdlib.h>
#include "Platform.h"
#include "APITypes.h"

#include <windows.h>
#include <winreg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <wininet.h>
#include <string.h>

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <atlbase.h>
#include "../HTTPCommon/HTTPProxyConfig.h"
using std::string;
using namespace boost::algorithm;
using namespace HTTP;

std::string Platform::getArchitectureName() {
#ifdef _WIN64
  return "win64";
#else
  return "win32";
#endif
}

string FBPlugin::lastError(const char* fnname) {
    DWORD errcode = GetLastError();
    char* buf = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errcode, 0, (LPSTR)&buf, 0, NULL);

    std::stringstream stream;
    stream << fnname << ": " << buf;
    LocalFree(buf);
    return stream.str();
}

void FBPlugin::throw_GetLastError(const char* fnname) {
    throw std::runtime_error(lastError(fnname).c_str());
}
using FBPlugin::throw_GetLastError;

bool Platform::pathIsHidden(const boost::filesystem::wpath& path_to_investigate) {
  DWORD attr = GetFileAttributesW(path_to_investigate.file_string().c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) return false;
  return (attr & FILE_ATTRIBUTE_HIDDEN);
}

bool Platform::pathIsSystem(const boost::filesystem::wpath& path_to_investigate) {
  DWORD attr = GetFileAttributesW(path_to_investigate.file_string().c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) return false;
  return (attr & FILE_ATTRIBUTE_SYSTEM);
}

bool Platform::pathIsShortcut(const boost::filesystem::wpath& path_to_investigate) {
  std::string ext = FB::wstring_to_utf8(path_to_investigate.extension());
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return (ext == ".lnk");
}

boost::filesystem::wpath Platform::resolveShortcut(const boost::filesystem::wpath& path_to_shortcut) {
  try {
    CComPtr<IShellLink> psl;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl);
    if (FAILED(hr)) {
      throw_GetLastError("resolveShortcut (CoCreateInstance(CLSID_ShellLink))");
    }
    CComQIPtr<IPersistFile> ppf(psl);
    if (!ppf) throw std::runtime_error("IPersistFile");
    if (FAILED(ppf->Load(path_to_shortcut.file_string().c_str(), STGM_READ))) throw_GetLastError("resolveShortcut (IPersistFile::Load)");
    WCHAR resPath[MAX_PATH];
    if (FAILED(psl->GetPath(resPath, MAX_PATH, NULL, 0 /*todo: might need a flag here*/ ))) throw_GetLastError("resolveShortcut (IShellLink::GetPath)");
    return boost::filesystem::wpath(resPath);

  } catch (const std::exception& e) {
    throw boost::filesystem::wfilesystem_error(e.what(), path_to_shortcut, boost::system::errc::make_error_code(boost::system::errc::bad_address));
  }
}


HTTPProxyConfig Platform::getSystemProxyConfig() {
  try {
    HTTPProxyConfig res;
    // The buffer that InternetQueryOption wants contains all of the strings after the struct in one
    // alloc, that's why this is variable size instead of just sizeof(INTERNET_PROXY_INFO)
    DWORD ipiSize = 0;
    InternetQueryOption(NULL, INTERNET_OPTION_PROXY, NULL, &ipiSize);
    boost::scoped_array<char> ipiBuf(new char[ipiSize]);
    INTERNET_PROXY_INFO* ipi = reinterpret_cast<INTERNET_PROXY_INFO*>(ipiBuf.get());

    if (! InternetQueryOption(NULL, INTERNET_OPTION_PROXY, ipiBuf.get(), &ipiSize)) throw_GetLastError("InternetQueryOption(INTERNET_OPTION_PROXY)");

    if (ipi->dwAccessType == INTERNET_OPEN_TYPE_PROXY) {
      // Proxy config looks like:
      // socks=socks_proxy_server:1080 https=https_proxy_server:4443 ftp=ftp_proxy_server:2222 http=http_proxy_server:3128
      // any of the key-value pairs may be omitted
      // or, if it's just a single proxy server (which is HTTP):
      // http_proxy_server:3128

      std::string proxyConfig(reinterpret_cast<const char*>(ipi->lpszProxy));

      std::vector<string> proxy_parts;
      split(proxy_parts, proxyConfig, is_any_of(" "));
      for (size_t i = 0; i < proxy_parts.size(); ++i) {
        std::vector<string> kvp;
        split(kvp, proxy_parts[i], is_any_of("="));
        if (kvp.empty()) continue;
        if (kvp.size() == 1) {
          // untyped proxy entry. just put http on the front of the array, then fall through to the other case
          kvp.insert(kvp.begin(), string("http"));
        }

        // this is a typed proxy entry. kvp[0] should be one of {socks,https,ftp,http}
        std::vector<string> host_parts;
        split(host_parts, kvp[1], is_any_of(":"));
        if (host_parts.size() < 2) continue; // what?
        res.port = boost::lexical_cast<int>(host_parts[1]);
        res.hostname = host_parts[0];

        if (kvp[0] == "socks") {
          res.type = HTTPProxyConfig::kSOCKS4Proxy;
        } else if ((kvp[0] == "http") || (kvp[0] == "https")) {
          res.type = HTTPProxyConfig::kHTTPProxy;
        }

        if (res.type != HTTPProxyConfig::kNoProxy) break; // don't parse the rest if we already got a proxy
      }
      // printf("Proxy config: type %d, host %s:%d\n", res.type, res.hostname.c_str(), res.port);
    }
    return res;
  } catch (const std::exception& e) {
    // TODO: This isn't a very good place to log this.
    printf("Platform::getSystemProxyConfig: error: %s\n", e.what());
    return HTTPProxyConfig();
  }
}

