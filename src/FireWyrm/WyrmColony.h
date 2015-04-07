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
    FB_FORWARD_PTR(WyrmBrowserHost);
    FB_FORWARD_PTR(WyrmSac);
    FB_FORWARD_PTR(AlienLarvae);
    class LocalWyrmling;
    using WyrmlingKey = std::pair<FW_INST, FW_INST>;
    
    class WyrmColony : boost::noncopyable
    {
    public:
        
        using CommandHandler = FB::VariantListPromise(WyrmColony::*)(FB::VariantList args);
        using CommandMap = std::map < std::string, CommandHandler > ;
        using StringDeferred = FB::Deferred < std::string > ;
        using StringPromise = FB::Promise < std::string > ;

        using SpawnMap = std::map < FW_INST, WyrmSacPtr > ;
        using WaitMap = std::map < uint32_t, StringDeferred > ;
        using LarvaeMap = std::map < std::pair< FW_INST, uint32_t>, AlienLarvaeWeakPtr > ;
        
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
        StringPromise sendCommand(FB::VariantList cmd);

        bool _scheduleAsyncCall(void(*func) (void*), void * userData);
        AlienLarvaePtr getLarvaeFor(const FW_INST spawnId, const uint32_t objId);

    private:
        //
        uint32_t getCommandId() {
            // Return the next unused command ID
            // Note that this is designed to eventually overflow and wrap around if needed; if somehow a long
            // running command is still running when it gets the next time around (astronomically unlikely)
            // the check to see if it is still in the waitMap will cause it to be skipped
            uint32_t cmdId = m_nextCmdId++;
            while (m_waitMap.find(cmdId) != m_waitMap.end()) {
                cmdId = m_nextCmdId++;
            }
            return cmdId;
        }

        // Messsage handlers
        FB::VariantListPromise New(FB::VariantList args);
        FB::VariantListPromise Destroy(FB::VariantList args);
        FB::VariantListPromise Enum(FB::VariantList args);
        FB::VariantListPromise Invoke(FB::VariantList args);
        FB::VariantListPromise GetP(FB::VariantList args);
        FB::VariantListPromise SetP(FB::VariantList args);
        FB::VariantListPromise RelObj(FB::VariantList args);

    public:
        // Host calls
        FB::variantPromise DoCommand(FB::VariantList args, WyrmBrowserHostPtr host = nullptr);

    protected:
        FB::variant makeLocalMethodWyrmling(WyrmBrowserHostPtr host, LocalWyrmling wyrmling, std::string method);
        
        FW_INST m_key;
        std::thread::id m_threadId;
        FWHostFuncs m_hFuncs;
        FW_INST m_nextSpawnId;
        uint32_t m_nextCmdId;
        FW_INST m_nextMethodId;
        SpawnMap m_spawnMap;
        WaitMap m_waitMap;
        LarvaeMap m_larvaeMap;
        static CommandMap cmdMap;
        static volatile uint32_t ColonyInitialized;
        static ColonyMap m_colonyMap;
        static void initCommandMap();
    };

} };

#endif // H_WYRMCOLONY

