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
#include "APITypes.h"
#include "Deferred.h"
#include "FireWyrm.h"
#include "APITypes.h"

namespace FB { namespace FireWyrm {
    FB_FORWARD_PTR(WyrmSpawn);

    class WyrmColony : boost::noncopyable
    {
        using CommandHandler = FB::VariantListPromise(WyrmColony::*)(FB::VariantList args);
        using CommandMap = std::map < std::string, CommandHandler > ;

        using SpawnMap = std::map < FW_INST, WyrmSpawnPtr > ;
    public:
        WyrmColony(FW_INST key);
        virtual ~WyrmColony();

        using ColonyMap = std::map<const FW_INST,WyrmColony*>;
        static WyrmColony* GetColony(const FW_INST);
        static FW_RESULT ReleaseColony(const FW_INST);
        void setFuncs(FWHostFuncs* hFuncs);
        void populateFuncs(FWColonyFuncs* cFuncs);
        FW_RESULT onCommand(const uint32_t cmdId, const std::string command);
        FW_RESULT onResponse(const uint32_t cmdId, const std::string response);
        void sendResponse(const uint32_t cmdId, FB::VariantList resp);

    private:
        // Messsage handlers
        FB::VariantListPromise New(FB::VariantList args);
        FB::VariantListPromise Destroy(FB::VariantList args);
        FB::VariantListPromise Enum(FB::VariantList args);
        FB::VariantListPromise Invoke(FB::VariantList args);
        FB::VariantListPromise GetP(FB::VariantList args);
        FB::VariantListPromise SetP(FB::VariantList args);
        FB::VariantListPromise RelObj(FB::VariantList args);

    protected:
        FW_INST m_key;
        std::thread::id m_threadId;
        FWHostFuncs m_hFuncs;
        FW_INST m_nextSpawnId;
        static CommandMap cmdMap;
        static volatile uint32_t ColonyInitialized;
        static ColonyMap m_colonyMap;
        static void initCommandMap();
        bool _scheduleAsyncCall(void(*func) (void*), void * userData);
    };

} };

#endif // H_WYRMCOLONY

