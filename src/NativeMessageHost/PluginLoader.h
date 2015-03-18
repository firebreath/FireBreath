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
};

#endif // PluginLoader_h__