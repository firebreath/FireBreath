/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 27, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 GradeCam, Inc and the Firebreath development team
\**********************************************************/

#include "../SystemHelpers.h"
#include "shlobj.h"

#ifndef REFKNOWNFOLDERID
typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID &
#endif

#ifndef DEFINE_KNOWN_FOLDER
#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C static const GUID name
DEFINE_KNOWN_FOLDER(FOLDERID_LocalAppDataLow,     0xA520A1A4, 0x1780, 0x4FF6, 0xBD, 0x18, 0x16, 0x73, 0x43, 0xC5, 0xAF, 0x16);
#undef DEFINE_KNOWN_FOLDER
#endif

using std::string;

typedef HRESULT (WINAPI*GetKnownFolderPathFunc)(REFKNOWNFOLDERID, DWORD, HANDLE hToken, PWSTR *ppszPath); // free *ppszPath with CoTaskMemFree

static bool useNewAPI(false);
static HMODULE library(NULL);
static GetKnownFolderPathFunc getKnownFolderPath(NULL);

namespace {
    void initLibrary() {
        library = LoadLibraryA("shell32.dll");
        assert(library != NULL);
        getKnownFolderPath = (GetKnownFolderPathFunc)GetProcAddress(library, "SHGetKnownFolderPath");
        useNewAPI = getKnownFolderPath != NULL;
    }

    std::string getFolderNew(REFKNOWNFOLDERID folder) {
        PWSTR pszPath;
        HRESULT hr = (*getKnownFolderPath)(folder, 0, NULL, &pszPath);

        std::wstring path(pszPath);
        CoTaskMemFree(pszPath);
        return FB::wstring_to_utf8(path);
    }

    std::string getFolderStd(int csIDL) {
        wchar_t rawPath[MAX_PATH];

        if (SHGetSpecialFolderPathW(NULL, rawPath, csIDL, FALSE)) {
            return FB::wstring_to_utf8(rawPath);
        } else {
            throw std::runtime_error("Could not find path");
        }
    }

    void closeLibrary() {
        FreeLibrary(library);
        library = NULL;
        getKnownFolderPath = NULL;
    }
};

string FB::System::getHomeDirPath()
{
    return getFolderStd(CSIDL_PROFILE);
}

string FB::System::getAppDataPath(const string& appName)
{
    std::string out;
    out = getFolderStd(CSIDL_APPDATA);
    return out + "\\" + appName;
}

string FB::System::getLocalAppDataPath(const string& appName)
{
    std::string out;
    initLibrary();
    if (useNewAPI) {
        out = getFolderNew(FOLDERID_LocalAppDataLow);
    } else {
        out = getFolderStd(CSIDL_LOCAL_APPDATA);
    }
    closeLibrary();
    return out + "\\" + appName;
}

string FB::System::getTempPath()
{
    wchar_t path[MAX_PATH];
    DWORD len;
    len = GetTempPathW(MAX_PATH, path);
    if (len > 0) {
        return FB::wstring_to_utf8(std::wstring(path, len));
    } else {
        throw std::runtime_error("Could not find path");
    }
}
