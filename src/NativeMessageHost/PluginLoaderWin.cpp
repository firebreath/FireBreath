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
#include "PluginLoaderWin.h"

std::unique_ptr<PluginLoader> PluginLoader::LoadPlugin(std::string mimetype) {
    std::string filename = getMimeHandlerPath(mimetype);

    if (!filename.length()) {
        throw new std::runtime_error("Could not find plugin that provides requested mime type");
    }

    return std::unique_ptr<PluginLoader>(new PluginLoaderWin(mimetype, filename));
}

std::string getMimeHandlerPath(std::string mimetype) {
    HKEY hPlugins,
         hPluginItem,
         hMimeTypesKey;
    char lpName[0xFF] = {0},        // buffer for subkey name
         lpData[0xFF] = {0};        // buffer for the value of the path key
    DWORD lpcName = 0xFF,           // size of lpName buffer (subkey name)
          lpcbData = 0xFF,          // size of the lpData buffer (path)
          dwIndex,                  // subkey counter
          dwsIndex,                 // subkey counter
          rc,                       // return code
          lrc;                      // loop return code
    std::vector<HKEY> hList = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    LPCWSTR PLUGIN_HIVE = TEXT("SOFTWARE\\MozillaPlugins");

    for (auto &hkey : hList) {
        rc = RegOpenKeyEx(hkey, PLUGIN_HIVE, 0, KEY_READ|KEY_WOW64_32KEY, &hPlugins);

        if (rc == ERROR_SUCCESS) {
            dwIndex = 0;  // reset the current subkey index
            while(!(lrc = RegEnumKeyEx(hPlugins, dwIndex++, lpName, &lpcName, NULL, NULL, NULL, NULL)) || lrc != ERROR_NO_MORE_ITEMS) {
                rc = RegOpenKeyEx(hPlugins, (LPWSTR) lpName, 0, KEY_READ, &hPluginItem);

                if (rc == ERROR_SUCCESS) {
                    rc = RegOpenKeyEx(hPluginItem, TEXT("MimeTypes"), 0, KEY_READ, &hMimeTypesKey); // get the MimeTypes key

                    if (rc == ERROR_SUCCESS) { // only continue with the item if it has a MimeTypes key
                        rc = RegQueryValueEx(hPluginItem, TEXT("Path"), NULL, NULL, (LPBYTE) lpData, &lpcbData); // Get the value of Path
                        dwsIndex = 0; // reset the current subkey index

                        while(!(rc = RegEnumKeyEx(hMimeTypesKey, dwsIndex++, (LPWSTR) lpName, &lpcName, NULL, NULL, NULL, NULL )) || rc != ERROR_NO_MORE_ITEMS) {

                            if (strcmp(lpName, mimetype.c_str()) == 0) {
                                RegCloseKey(hMimeTypesKey);
                                RegCloseKey(hPlugins);
                                RegCloseKey(hPluginItem);
                                return lpData;
                            }

                            lpcName = 0xFF; // reset lpcName buffer size
                        }

                        RegCloseKey(hMimeTypesKey);
                        RegCloseKey(hPluginItem);
                    }
                }

                lpcName = 0xFF; // reset lpcName buffer size
                RegCloseKey(hPluginItem); // close hPluginItem
            }
        }

        RegCloseKey(hPlugins);
    }
    return ""; // no provider found
}

PluginLoaderWin::PluginLoaderWin(std::string mimetype, std::string filename)
    : PluginLoader(mimetype), m_module(nullptr), initFn(nullptr), finitFn(nullptr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::wstring libPath = utf8_conv.from_bytes(filename);
    m_module = LoadLibrary(libPath.c_str());

    if (!m_module) {
        throw new std::runtime_error("Could not load file");
    }

    initFn = reinterpret_cast<InitFnPtr>(GetProcAddress(m_module, "FireWyrm_Init"));
    finitFn = reinterpret_cast<FinitFnPtr>(GetProcAddress(m_module, "FireWyrm_Finit"));

    if (!initFn || !finitFn) {
        throw new std::runtime_error("Could not find entry points");
    }
}

PluginLoaderWin::~PluginLoaderWin() {
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
