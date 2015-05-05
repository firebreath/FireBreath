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
#pragma once
#ifndef PluginLoader_h__
#define PluginLoader_h__

#include "HostMain.h"
#include "FireWyrm.h"
#include <algorithm>
#include <string>
#include <memory>

struct PluginInfo {
    std::string name;
    std::string description;
    std::string path;
    std::string product_name;
    std::string vendor;
    std::string version;
    std::vector<std::string> mime_types;
    bool providesMimeType(std::string mimetype) {
        return (find(mime_types.begin(), mime_types.end(), mimetype) != mime_types.end());
    }
};

class PluginList : public std::vector<PluginInfo> {
    public:
    PluginList::iterator findByMimetype(std::string mime) {
        for(auto it = begin(); it != end(); ++it) {
            if (it->providesMimeType(mime))
                return it;
        }
        return end();
    };
};

class PluginLoader
{
public:
    static std::unique_ptr<PluginLoader> LoadPlugin(std::string mimetype);
    static PluginList getPluginList();

    PluginLoader(std::string mimetype) {};
    virtual ~PluginLoader() { };

    virtual FW_RESULT Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) = 0;
    virtual FW_RESULT Finit() = 0;

    virtual std::string getPluginName() = 0;
};

#endif // PluginLoader_h__
