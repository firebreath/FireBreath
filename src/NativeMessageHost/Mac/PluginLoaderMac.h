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
#ifndef PluginLoaderMac_h__
#define PluginLoaderMac_h__

#include <vector>
#include "PluginLoader.h"
#include "FireWyrm.h"

class PluginLoaderMac : public PluginLoader
{
public:
    PluginLoaderMac(std::string mimetype, std::string filename, std::string name = "");
    virtual ~PluginLoaderMac();

    FW_RESULT Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) override;
    FW_RESULT Finit() override;
    std::string getPluginName() override {
        // TODO: Populate basic plugin metadata when searching, then have it available here
        return name;
    }

private:
    void* m_module;
    std::string name;

    using InitFnPtr = FW_RESULT(*)(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs);
    using FinitFnPtr = FW_RESULT(*)();

    InitFnPtr initFn;
    FinitFnPtr finitFn;
};

#endif // PluginLoaderMac_h__
