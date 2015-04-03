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
#include <dlfcn.h>
#include <locale>
#include "npapi.h"
#include "PluginLoaderMac.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

typedef std::vector<boost::filesystem::path> vec;

using namespace boost;
using namespace boost::filesystem;
using boost::property_tree::ptree;

std::unique_ptr<PluginLoader> PluginLoader::LoadPlugin(std::string mimetype) {
    PluginList plugins(getPluginList());

    auto fnd = plugins.findByMimetype(mimetype);
    if (fnd == plugins.end()) {
        throw new std::runtime_error("No registered plugins detected");
    }

    return std::unique_ptr<PluginLoader>(new PluginLoaderMac(mimetype, fnd->path));
}

PluginList PluginLoader::getPluginList() {
    PluginList result;
    std::string home = getenv("HOME");
    std::string user_plugin_path = home + "/Library/Internet Plug-Ins";
    std::string system_plugin_path = "/Library/Internet Plug-Ins";
    std::vector<std::string> plugin_paths = {user_plugin_path, system_plugin_path};
    std::string plist_path,
                key;
    ptree pt;

    for (auto &pp : plugin_paths) {
        path p (pp);
        if (exists(p)) {
            if (is_directory(p)) {
                vec v;
                std::copy(directory_iterator(p), directory_iterator(), back_inserter(v));
                for (vec::const_iterator it (v.begin()); it != v.end(); ++it) {

                    plist_path = it->string() + "/Contents/Info.plist";
                    path plistp (plist_path);

                    if (exists(plistp)) {
                        PluginInfo plugin;
                        plugin.path = it->string();

                        read_xml(plistp.string().c_str(), pt);

                        ptree ptsub = pt.get_child("plist.dict");
                        boost::property_tree::ptree::iterator child_it;

                        for (child_it=ptsub.begin(); child_it != ptsub.end(); ++child_it) {
                            key = child_it->second.data();
                            if (key == "WebPluginName") {
                                ++child_it;
                                if (child_it != ptsub.end())
                                    plugin.name = child_it->second.data();
                            } else if (key == "WebPluginDescription") {
                                ++child_it;
                                if (child_it != ptsub.end())
                                    plugin.description = child_it->second.data();
                            } else if (key == "CFBundleShortVersionString") {
                                ++child_it;
                                if (child_it != ptsub.end())
                                    plugin.version = child_it->second.data();
                            } else if (key == "WebPluginMIMETypes") {
                                ++child_it;
                                key = child_it->first.data();
                                if (child_it != ptsub.end() && key == "dict") {
                                    for (auto mime : child_it->second) {
                                        key = mime.first.data();
                                        if (key == "key") {
                                            plugin.mime_types.push_back(mime.second.data());
                                        }
                                    }
                                }
                            }
                        }
                        result.emplace_back(plugin);
                    }
                }
            }
        }
    }
    return result;
}

PluginLoaderMac::PluginLoaderMac(std::string mimetype, std::string filename)
    : PluginLoader(mimetype), m_module(nullptr), initFn(nullptr), finitFn(nullptr) {
    m_module = dlopen(filename.c_str(), RTLD_LAZY);

    if (!m_module) {
        throw new std::runtime_error("Could not load file");
    }

    initFn = reinterpret_cast<InitFnPtr>(dlsym(m_module, "FireWyrm_Init"));
    finitFn = reinterpret_cast<FinitFnPtr>(dlsym(m_module, "FireWyrm_Finit"));

    if (!initFn || !finitFn) {
        throw new std::runtime_error("Could not find entry points");
    }
}

PluginLoaderMac::~PluginLoaderMac() {
    if (m_module) {
        dlclose(m_module);
    }
}

FW_RESULT PluginLoaderMac::Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) {
    return (*initFn)(hFuncs, cFuncs);
}

FW_RESULT PluginLoaderMac::Finit() {
    return (*finitFn)();
}
