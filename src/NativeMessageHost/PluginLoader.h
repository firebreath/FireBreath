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

#include "FireWyrm.h"
#include <string>
#include <memory>

class PluginLoader
{
public:
    static std::unique_ptr<PluginLoader> LoadPlugin(std::string mimetype);

    PluginLoader(std::string mimetype) {};
    virtual ~PluginLoader() {
        Finit();
    };

    virtual FW_RESULT Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) = 0;
    virtual FW_RESULT Finit() = 0;

    virtual std::string getPluginName() = 0;
};

struct PluginInfo {
    std::string name;
    std::string description;
    std::string path;
    std::string product_name;
    std::string vendor;
    std::vector<std::string> mime_types;
    bool providesMimeType(std::string mimetype) {
        if (find(mime_types.begin(), mime_types.end(), mimetype) != mime_types.end())
          return true;
        return false;
    }
};

template <class T>
class PluginList : public std::vector<T> {
    public:
    std::string find_mime(const std::string& mime) {
        for(PluginList<PluginInfo*>::iterator it = this->begin(); it != this->end(); ++it) {
            if ((*it)->providesMimeType(mime.c_str()))
                return (*it)->path;
        }
        return "";
    };
};

#endif // PluginLoader_h__
