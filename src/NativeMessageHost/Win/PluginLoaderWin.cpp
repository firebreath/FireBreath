/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 18, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include <codecvt>
#include <locale>
#include <boost/filesystem.hpp>
#include "PluginLoaderWin.h"

using boost::filesystem::path;
using boost::filesystem::exists;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE 32767

std::unique_ptr<PluginLoader> PluginLoader::LoadPlugin(std::string mimetype) {
    PluginList plugins(getPluginList());

    auto fnd = plugins.findByMimetype(mimetype);
    if (fnd == plugins.end()) {
        throw std::runtime_error("No registered plugins detected");
    }

    return std::unique_ptr<PluginLoader>(new PluginLoaderWin(mimetype, fnd->path, fnd->name));
}

PluginList PluginLoader::getPluginList() {
    PluginList result;
    HKEY hPlugins,
         hPluginItem,
         hMimeTypesKey;
    TCHAR lpName[MAX_KEY_LENGTH],   // buffer for subkey name
          lpData[MAX_VALUE];        // buffer for the value of the path key
    DWORD lpcName = MAX_KEY_LENGTH, // size of lpName buffer (subkey name)
          lpcbData = MAX_VALUE,     // size of the lpData buffer (path)
          dwIndex,                  // subkey counter
          dwsIndex,                 // subkey counter
          rc,                       // return code
          lrc;                      // loop return code
    std::vector<HKEY> hList = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    LPCWSTR PLUGIN_HIVE = TEXT("SOFTWARE\\MozillaPlugins");
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

    for (auto &hkey : hList) {
        rc = RegOpenKeyEx(hkey, PLUGIN_HIVE, 0, KEY_READ|KEY_WOW64_32KEY, &hPlugins);

        if (rc == ERROR_SUCCESS) {
            dwIndex = 0;  // reset the current subkey index
            lpcName = MAX_KEY_LENGTH; // reset lpcName buffer size
            while(!(lrc = RegEnumKeyEx(hPlugins, dwIndex++, lpName, &lpcName, NULL, NULL, NULL, NULL)) || lrc != ERROR_NO_MORE_ITEMS) {
                lpcName = MAX_KEY_LENGTH; // reset lpcName buffer size
                rc = RegOpenKeyEx(hPlugins, lpName, 0, KEY_READ, &hPluginItem);
                if (rc == ERROR_SUCCESS) {
                    PluginInfo plugin;
                    plugin.name = utf8_conv.to_bytes(lpName);

                    rc = RegQueryValueEx(hPluginItem, TEXT("FireWyrm"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS) {
                        auto fwSupport = utf8_conv.to_bytes(lpData);
                        if (!fwSupport.length() || fwSupport != "true") {
                            // This isn't a firewyrm compatible plugin
                            continue;
                        }
                    } else {
                        //Not a firewyrm compatible plugin, skip
                        continue;
                    }

                    rc = RegQueryValueEx(hPluginItem, TEXT("Description"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS)
                        plugin.description = utf8_conv.to_bytes(lpData);
                    lpcbData = MAX_VALUE;

                    rc = RegQueryValueEx(hPluginItem, TEXT("Path"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS)
                        plugin.path = utf8_conv.to_bytes(lpData);
                    if (!exists(lpData)) {
                        // If the plugin file isn't there then don't display it as an option
                        continue;
                    }
                    lpcbData = MAX_VALUE;

                    rc = RegQueryValueEx(hPluginItem, TEXT("ProductName"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS)
                        plugin.product_name = utf8_conv.to_bytes(lpData);
                    lpcbData = MAX_VALUE;

                    rc = RegQueryValueEx(hPluginItem, TEXT("Vendor"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS)
                        plugin.vendor = utf8_conv.to_bytes(lpData);
                    lpcbData = MAX_VALUE;

                    rc = RegQueryValueEx(hPluginItem, TEXT("Version"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                    if (rc == ERROR_SUCCESS)
                        plugin.version = utf8_conv.to_bytes(lpData);

                    rc = RegOpenKeyEx(hPluginItem, TEXT("MimeTypes"), 0, KEY_READ, &hMimeTypesKey); // get the MimeTypes key
                    if (rc == ERROR_SUCCESS) {
                        dwsIndex = 0; // reset the current subkey index
                        lpcName = MAX_KEY_LENGTH; // reset lpcName buffer size
                        while(!(rc = RegEnumKeyEx(hMimeTypesKey, dwsIndex++, lpName, &lpcName, NULL, NULL, NULL, NULL )) || rc != ERROR_NO_MORE_ITEMS) {
                            if (rc == ERROR_SUCCESS) {
                                plugin.mime_types.push_back(utf8_conv.to_bytes(lpName));
                            }
                            lpcName = MAX_KEY_LENGTH; // reset lpcName buffer size
                        }
                        RegCloseKey(hMimeTypesKey);
                    }

                    RegCloseKey(hPluginItem); // close hPluginItem
                    result.emplace_back(plugin);
                }
            }
        }

        RegCloseKey(hPlugins);
    }
    return result;
}

PluginLoaderWin::PluginLoaderWin(std::string mimetype, std::string filename, std::string name)
    : PluginLoader(mimetype), m_module(nullptr), m_pluginName(name), initFn(nullptr), finitFn(nullptr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::wstring libPath = utf8_conv.from_bytes(filename);
    m_module = LoadLibrary(libPath.c_str());

    if (!m_module) {
        throw std::runtime_error("Could not load file");
    }

    initFn = reinterpret_cast<InitFnPtr>(GetProcAddress(m_module, "FireWyrm_Init"));
    finitFn = reinterpret_cast<FinitFnPtr>(GetProcAddress(m_module, "FireWyrm_Finit"));

    if (!initFn || !finitFn) {
        throw std::runtime_error("Could not find entry points");
    }
}

PluginLoaderWin::~PluginLoaderWin() {
    Finit();
    if (m_module) {
        FreeLibrary(m_module);
    }
}

FW_RESULT PluginLoaderWin::Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) {
    return (*initFn)(hFuncs, cFuncs);
}

FW_RESULT PluginLoaderWin::Finit() {
    return (*finitFn)();
}
