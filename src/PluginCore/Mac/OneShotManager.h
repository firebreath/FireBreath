
/**********************************************************\
 Original Author: Georg Fritzsche
 
 Created:    Nov 6, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche and the Firebreath development team
 \**********************************************************/

#pragma once
#ifndef FB_H_ONESHOTMANAGER
#define FB_H_ONESHOTMANAGER

#include <map>
#include <boost/utility.hpp>

#include "APITypes.h"
#include "SafeQueue.h"


namespace FB
{
    class OneShotManager : public boost::noncopyable
    {
    public:
        ~OneShotManager();
        
        typedef void (*OneShotCallback)(void*, uint32_t);
        
        void push(void*, OneShotCallback);
        void clear(void*);
        
        void shoot();
        
        static OneShotManager& getInstance();
        
    private:
        OneShotManager();
        
        typedef std::pair<void*, OneShotCallback> SinkPair;
        typedef FB::SafeQueue<SinkPair> SinkQueue;
        SinkQueue m_sinks;
        mutable boost::mutex m_mutex;
    };
}

#endif

