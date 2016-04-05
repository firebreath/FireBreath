/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 20, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <dlfcn.h>
#include <locale>
#include "npapi.h"
#include "PluginLoaderLin.h"

typedef const char* (*func_ptr)();
typedef const char* (*NP_GetValue_ptr)(void*, int, void*);
typedef std::vector<boost::filesystem::path> vec;

using namespace boost;
using namespace boost::filesystem;

std::unique_ptr<PluginLoader> PluginLoader::LoadPlugin(std::string mimetype) {
    PluginList plugins(getPluginList());

    auto fnd = plugins.findByMimetype(mimetype);
    if (fnd == plugins.end()) {
        throw std::runtime_error("No registered plugins detected");
    }

    return std::unique_ptr<PluginLoader>(new PluginLoaderLin(mimetype, fnd->path));
}

PluginList PluginLoader::getPluginList() {
    PluginList result;
    std::string home = getenv("HOME");
    std::string user_plugin_path = home + "/.mozilla/plugins";
    std::string system_plugin_path = "/usr/lib/mozilla/plugins";
    std::string system64_plugin_path = "/usr/lib64/mozilla/plugins";
    std::vector<std::string> plugin_paths = {user_plugin_path, system_plugin_path, system64_plugin_path};
    boost::regex mime_regex("(\\w*?[/][\\w\\-\\.]+?)(?:[;\\:])|$");
    void *dlo_handle;

    func_ptr NP_GetMIMEDescription;
    func_ptr NP_GetPluginVersion;
    NP_GetValue_ptr NP_GetValue;
    const char* err;
    char* name_value;
    char* desc_value;
    std::string version,
                mime_desc;

    for (auto &pp : plugin_paths) {
        path p (pp);
        if (exists(p)) {
            if (is_directory(p)) {
                vec v;
                std::copy(directory_iterator(p), directory_iterator(), back_inserter(v));
                for (vec::const_iterator it (v.begin()); it != v.end(); ++it) {
                    if ((dlo_handle = dlopen (it->string().c_str(), RTLD_LAZY))) {
                        PluginInfo plugin;
                        plugin.path = it->string();

                        dlerror(); // clear dlerror
                        NP_GetValue = (NP_GetValue_ptr) dlsym(dlo_handle, "NP_GetValue");
                        if (!(err = dlerror())) {
                            NP_GetValue(NULL, NPPVpluginNameString, &(name_value));
                            plugin.name = name_value;

                            NP_GetValue(NULL, NPPVpluginDescriptionString, &(desc_value));
                            plugin.description = desc_value;
                        }

                        dlerror(); // clear dlerror
                        NP_GetPluginVersion = (func_ptr) dlsym(dlo_handle, "NP_GetPluginVersion");
                        if (!(err = dlerror())) {
                            version = NP_GetPluginVersion();
                            plugin.version = version.c_str();
                        }

                        dlerror(); // clear dlerror
                        NP_GetMIMEDescription = (func_ptr) dlsym(dlo_handle, "NP_GetMIMEDescription");
                        if (!(err = dlerror())) {
                            mime_desc = NP_GetMIMEDescription();
                            boost::sregex_token_iterator iter(mime_desc.begin(), mime_desc.end(), mime_regex, 1),
                                                         end;
                            for(; iter != end; ++iter) {
                                if (find(plugin.mime_types.begin(), plugin.mime_types.end(), *iter) == plugin.mime_types.end()) {
                                    plugin.mime_types.push_back(*iter);
                                }
                            }
                        }
                        result.emplace_back(plugin);
                        dlclose(dlo_handle);
                    }
                }
            }
        }
    }
    return result;
}

PluginLoaderLin::PluginLoaderLin(std::string mimetype, std::string filename)
    : PluginLoader(mimetype), m_module(nullptr), initFn(nullptr), finitFn(nullptr) {
    m_module = dlopen(filename.c_str(), RTLD_LAZY);

    if (!m_module) {
        throw std::runtime_error("Could not load file");
    }

    initFn = reinterpret_cast<InitFnPtr>(dlsym(m_module, "FireWyrm_Init"));
    finitFn = reinterpret_cast<FinitFnPtr>(dlsym(m_module, "FireWyrm_Finit"));

    if (!initFn || !finitFn) {
        throw std::runtime_error("Could not find entry points");
    }
}

PluginLoaderLin::~PluginLoaderLin() {
    if (m_module) {
        dlclose(m_module);
    }
}

FW_RESULT PluginLoaderLin::Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) {
    return (*initFn)(hFuncs, cFuncs);
}

FW_RESULT PluginLoaderLin::Finit() {
    return (*finitFn)();
}
