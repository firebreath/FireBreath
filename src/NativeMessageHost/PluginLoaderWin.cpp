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
    std::string filename = "C:\\code\\FireBreath2\\build\\firebreath\\bin\\FBTestPlugin\\Debug\\npFBTestPlugin.dll";

    return std::unique_ptr<PluginLoader>(new PluginLoaderWin(mimetype, filename));
}

PluginLoaderWin::PluginLoaderWin(std::string mimetype, std::string filename)
    : PluginLoader(mimetype), m_module(nullptr), initFn(nullptr), finitFn(nullptr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::wstring libPath = utf8_conv.from_bytes(filename);
    m_module = LoadLibrary(libPath.c_str());

    if (!m_module) {
        throw new std::runtime_error("Could not load file");
    }

    initFn = GetProcAddress(m_module, L"FireWyrm_Init");
    finitFn = GetProcAddress(m_module, L"FireWyrm_Finit");

    if (!initFn || !finitFn) {
        throw new std::runtime_error("Could not find entrypoints");
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
