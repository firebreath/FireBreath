/*********************************************************************\
    Original Author: Richard Bateman

    Created:    Jan 18, 2015
    License:    Dual license model; choose one of two:
                New BSD License
                http://www.opensource.org/licenses/bsd-license.php
                - or -
                GNU Lesser General Public License, version 2.1
                http://www.gnu.org/licenses/lgpl-2.1.html

    Copyright 2015 Richard Bateman and the Firebreath development team
\*********************************************************************/

#pragma once
#ifndef H_WYRMCOLONY
#define H_WYRMCOLONY

#include <boost/noncopyable.hpp>
#include <thread>
#include "Deferred.h"
#include "FireWyrm.h"
#include "APITypes.h"

namespace FB { namespace FireWyrm {
    FB_FORWARD_PTR(WyrmSpawn);
    class WyrmColony : boost::noncopyable
    {
    public:
        WyrmColony();
        virtual ~WyrmColony();

        using ColonyMap = std::map<const FW_INST,WyrmColony*>;
        static WyrmColony* GetColony(const FW_INST);
        static FW_RESULT ReleaseColony(const FW_INST);
        void setFuncs(FWHostFuncs* hFuncs);
        void populateFuncs(FWColonyFuncs* cFuncs);
        FW_RESULT onCommand(std::string command);
        FW_RESULT onResponse(std::string response);

    protected:
        std::thread::id m_threadId;
        FWHostFuncs m_hFuncs;
        static volatile uint32_t ColonyInitialized;
        static ColonyMap m_colonyMap;
    };

} };

#endif // H_WYRMCOLONY

