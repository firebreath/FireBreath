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
#include <iostream>
#include "PluginLoaderMac.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/iterator_range.hpp>

typedef std::vector<boost::filesystem::path> vec;

using namespace boost;
using namespace boost::filesystem;
using boost::property_tree::ptree;

std::unique_ptr<PluginLoader> PluginLoader::LoadPlugin(std::string mimetype) {
    PluginList plugins(getPluginList());

    auto fnd = plugins.findByMimetype(mimetype);
    if (fnd == plugins.end()) {
        throw std::runtime_error("No registered plugins detected");
    }
	std::ostringstream oss;
	oss << "Loading plugin from: " << fnd->path;
	log(oss.str());
    
    return std::unique_ptr<PluginLoader>(new PluginLoaderMac(mimetype, fnd->path, fnd->name));
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

PluginList PluginLoader::getPluginList() {
    PluginList result;
    path home(getenv("HOME"));
    path sysroot("/");
    path user_plugin_path = home / "Library" / "Internet Plug-Ins";
    path system_plugin_path = sysroot / "Library" / "Internet Plug-Ins";
    std::vector<path> plugin_paths = {user_plugin_path, system_plugin_path};
    std::string key;
    ptree pt;

    for (auto &p : plugin_paths) {
        if (exists(p) && is_directory(p)) {
            auto dirList = boost::make_iterator_range(directory_iterator(p), directory_iterator());
            for (auto pEntry : dirList) {
                path pluginDir(pEntry);
                if (!hasEnding(pluginDir.string(), ".plugin")) {
                    // Only consider plugins
                    continue;
                }
                path plistp = pluginDir / "Contents" / "Info.plist";
                if (!exists(plistp)) {
                    // plist file is required
                    continue;
                }

                PluginInfo plugin;

                read_xml(plistp.string().c_str(), pt);

                ptree ptsub = pt.get_child("plist.dict");
                boost::property_tree::ptree::iterator child_it;
                bool isFBPlugin = false;

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
                    } else if (key == "WebPluginSupportsFireWyrm") {
                        ++child_it;
                        if (child_it != ptsub.end())
                            isFBPlugin = (child_it->second.data() == std::string("YES"));
                    } else if (key == "CFBundleExecutable") {
                        ++child_it;
                        if (child_it != ptsub.end())
                            plugin.path = (pluginDir / "Contents" / "MacOS" / child_it->second.data()).string();
                    } else if (key == "CFBundleVersion") {
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
                // If we didn't find a path or this isn't a FireWyrm compatible plugin, ignore it
                if (plugin.path.empty() || !isFBPlugin)
                    continue;
                result.emplace_back(plugin);
            }
        }
    }
    return result;
}

PluginLoaderMac::PluginLoaderMac(std::string mimetype, std::string filename, std::string name)
    : PluginLoader(mimetype), m_module(nullptr), name(name), initFn(nullptr), finitFn(nullptr) {
    m_module = dlopen(filename.c_str(), RTLD_LAZY);

    if (!m_module) {
		throw std::runtime_error(dlerror());
    }

    initFn = reinterpret_cast<InitFnPtr>(dlsym(m_module, "FireWyrm_Init"));
    finitFn = reinterpret_cast<FinitFnPtr>(dlsym(m_module, "FireWyrm_Finit"));

    if (!initFn || !finitFn) {
        throw std::runtime_error("Could not find entry points");
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
